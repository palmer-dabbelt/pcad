// See LICENSE for details

#include "compile.h++"
#include <pcad/netlist/macro.h++>
#include <pcad/rtlir/module.h++>
#include <pcad/rtlir/instance.h++>
#include <pcad/util/collection.h++>
#include <tuple>
#include <simple_match/simple_match.hpp>
#include <pcad/util/assert.h++>
#include <cmath>
#include <map>
#include <unordered_map>
using namespace pcad;
using namespace simple_match;
using namespace simple_match::placeholders;

/* These functions */
static size_t memory_cost(const rtlir::module::ptr& module);

std::vector<rtlir::circuit::ptr> passes::compile(
    const std::vector<netlist::macro::ptr>& to_compile,
    const std::vector<netlist::macro::ptr>& compile_to)
{
    auto out = std::vector<rtlir::circuit::ptr>();
    for (const auto& m: to_compile) {
        match(m,
            someptr<netlist::memory_macro>(), [&](const auto& mm) {
                out.push_back(
                    compile(
                        mm,
                        putil::collection::filter<netlist::memory_macro>(compile_to)
                    )
                );
            }
        );
    }
    return out;
}

rtlir::circuit::ptr passes::compile(
    const netlist::memory_macro::ptr& to_compile,
    const std::vector<netlist::memory_macro::ptr>& compile_to)
{
    /* Select the best macro that we'll use to implement */
    auto vmacro = [&]() {
        auto best_circuit = rtlir::circuit::ptr(nullptr);
        auto best_macro = netlist::memory_macro::ptr(nullptr);
        auto best_area = (size_t)(-1);

        for (const auto& to_try: compile_to) {
            /* A quick cost function (that must be kept in sync with
             * memory_cost()) that attempts to avoid compiling unncessary
             * memories.  This is a lower bound on the cost of compiling a
             * memory: it assumes 100% bit-cell utilization when mapping. */
            size_t quick_cost =
                100 * (to_compile->width() * to_compile->depth()) / (to_try->width() * to_try->depth())
                + (to_compile->width() * to_compile->depth());
            if (quick_cost > best_area)
                continue;

            auto compiled = compile(to_compile, to_try);

            auto compiled_area = compiled == nullptr
                ? (size_t)(-1)
                : memory_cost(compiled->top());

            if (compiled != nullptr)
                std::cerr << "INFO: compiling " << to_compile->name() << " using " << to_try->name() << " costs " << std::to_string(compiled_area) << "\n";

            if (best_area > compiled_area) {
                best_circuit = compiled;
                best_macro = to_try;
                best_area = compiled_area;
            }
        }

        return best_macro;
    }();

    if (vmacro != nullptr)
        std::cerr << "INFO: best compilation of " << to_compile->name() << " is " << vmacro->name() << "\n";

    return compile(to_compile, vmacro);
}

rtlir::circuit::ptr passes::compile(
    const netlist::memory_macro::ptr& to_compile,
    const netlist::memory_macro::ptr& compile_to)
{
    if (compile_to == nullptr)
        return std::make_shared<rtlir::circuit>(to_compile);

    auto black_box = std::make_shared<netlist::memory_blackbox>(compile_to);

    auto name2port = std::unordered_map<std::string, rtlir::port::ptr>();
    auto ports = std::vector<rtlir::port::ptr>();
    auto portify = [&](const rtlir::port::ptr& mmp) {
        if (mmp == nullptr)
            return mmp;

        auto l = name2port.find(mmp->name());
        if (l != name2port.end())
            return l->second;

        name2port[mmp->name()] = mmp;
        ports.push_back(mmp);
        return mmp;
    };

    /* FIXME: This just assumes the Chisel and vendor ports are in the same
     * order, but I'm starting with what actually gets generated. */
    if (to_compile->mem_ports().size() != compile_to->mem_ports().size()) {
        std::cerr << "INFO: unable to compile " << to_compile->name() << " using " << compile_to->name() << ": port count must match\n";
        return std::make_shared<rtlir::circuit>(to_compile);
    }
    auto memory_port_order = [](const auto& p) {
        if (p->input_port_name().valid() && p->output_port_name().valid())
            return 0;
        if (!p->input_port_name().valid() && p->output_port_name().valid())
            return 1;
        if (p->input_port_name().valid() && !p->output_port_name().valid())
            return 2;
        return 3;
    };
    auto compare_memory_port = [&](const auto& p, const auto& q) {
        return memory_port_order(p) > memory_port_order(q);
    };
    auto paired_memory_ports = putil::collection::map_zip(
        putil::collection::sort(
            to_compile->mem_ports(),
            compare_memory_port
        ),
        putil::collection::sort(
            compile_to->mem_ports(),
            compare_memory_port
        ),
        [&](const auto& tcp, const auto& ctp) {
            return std::make_pair(tcp, ctp);
        }
    );

    auto parallel_pairs = [&](){
        auto out = std::vector<std::pair<int, int>>();
        int i, last;
        for (i = 0, last = 0; i < to_compile->width(); ++i) {
            /* Every memory is going to have to fit at least a single bit. */
            if (i <= last + 1) continue;

            /* It's possible that we rolled over a memory's width here, if so
             * generate one. */
            if ((i - last) % compile_to->width() == 0) {
                out.push_back(std::make_pair(last, i));
                last = i;
                continue;
            }

            /* FIXME: This is a mess, I must just be super confused. */
            for (const auto& pp: paired_memory_ports) {
                if (pp.second->mask_gran().valid() && pp.second->mask_gran().data() == 1)
                    continue;

                if (pp.first->mask_gran().valid() && i % pp.first->mask_gran().data() == 0) {
                    out.push_back(std::make_pair(last, i));
                    last = i;
                    continue;
                }

                util::assert(
                    pp.second->mask_gran().valid() == false
                    || pp.second->mask_gran().data() == 1
                    || pp.second->mask_gran().data() == pp.second->bit_width().data(),
                    "I only support bit-mask (or unmasked) target memories"
                );
            }
        }
        out.push_back(std::make_pair(last, i));
        return out;
    }();
    std::cerr << "INFO: Mapped " << to_compile->name() << " to " << compile_to->name() << " using widths:\n";
    for (const auto& pp: parallel_pairs)
        std::cerr << "  [" << std::to_string(pp.first) << ", " << std::to_string(pp.second) << ")\n";

    auto logic = std::vector<rtlir::statement::ptr>();

    auto instances = std::vector<rtlir::instance::ptr>();
    auto wires = std::vector<rtlir::wire::ptr>();
    auto statements = std::vector<rtlir::statement::ptr>();
    auto selects = std::multimap<rtlir::port::ptr, std::tuple<rtlir::wire::ptr, int, rtlir::statement::ptr>>();

    for (auto serial = 0; serial < to_compile->depth(); serial += compile_to->depth()) {
        auto si = serial / compile_to->depth();

        auto cats = std::multimap<rtlir::port::ptr, std::tuple<rtlir::wire::ptr, int, rtlir::wire::ptr>>();

        for (size_t pi = 0; pi < parallel_pairs.size(); ++pi) {
            auto parallel_lower = parallel_pairs[pi].first;
            auto parallel_upper = parallel_pairs[pi].second;
            auto connects = std::vector<rtlir::port_connect_statement::ptr>();

            /* All the helper functions above are garbage, these are the ones that actually work. */
            auto assign = [&](const rtlir::port::ptr& target, const rtlir::statement::ptr& source) {
                if (target->width() < source->width()) {
#ifdef SMALL_PORT_WARNING
                    std::cerr << "WARNING: Assigned large statement to small port " << target->name() << "\n";
#endif
                    auto as = std::make_shared<rtlir::port_connect_statement>(target, source);
                    connects.push_back(as);
                    return as;
                } else if (target->width() > source->width()) {
                    auto as = std::make_shared<rtlir::port_connect_statement>(
                        target,
                        std::make_shared<rtlir::cat_statement>(
                            source,
                            std::make_shared<rtlir::literal_statement>(
                                0,
                                target->width() - source->width()
                            )
                        )
                    );
                    connects.push_back(as);
                    return as;
                } else {
                    auto as = std::make_shared<rtlir::port_connect_statement>(target, source);
                    connects.push_back(as);
                    return as;
                }
            };

            auto assign_port = [&](const rtlir::port::ptr& target, const rtlir::wire::ptr& source) {
                return assign(target, std::make_shared<rtlir::wire_statement>(source));
            };

#if 0
            auto wire_assign_statement = [&](const rtlir::wire::ptr& target, const rtlir::statement::ptr& source) {
                auto as = std::make_shared<rtlir::connect_statement>(target, source);
                logic.push_back(as);
                return as;
            };
#endif

            auto wire_assign_wire = [&](const rtlir::wire::ptr& target, const rtlir::wire::ptr& source) {
                auto as = std::make_shared<rtlir::connect_statement>(target, source);
                logic.push_back(as);
                return as;
            };

            auto mktemp = [&](const std::string& base_name, const int& width) {
                auto w = std::make_shared<rtlir::wire>(
                    base_name + "_" + std::to_string(si) + "_" + std::to_string(pi),
                    width
                );
                wires.push_back(w);
                return w;
            };

            /* It's legal to have the same port multiple times when mapping
             * memories.  This ensures that we safely assign these sorts of
             * ports. */
            auto assigned_ports = std::unordered_map<std::string, std::string>();
            auto ports_are_already_assigned = [&](const rtlir::port::ptr& outer, const rtlir::port::ptr& inner) {
                if (outer == nullptr && inner == nullptr)
                    return true;

                if (outer == nullptr || inner == nullptr)
                    return false;

                auto l = assigned_ports.find(inner->name());
                if (l == assigned_ports.end()) {
                    assigned_ports[inner->name()] = outer->name();
                    return false;
                }

                if (l->second != outer->name()) {
                    std::cerr << "ERROR: Elided second port assignment, but it's to a different port\n";
                    std::cerr << "  inner port name: " << inner->name() << "\n";
                    std::cerr << "  first outer port name: " << l->second << "\n";
                    std::cerr << "  second outer port name: " << outer->name() << "\n";
                    abort();
                }

                return true;
            };

 
            for (const auto& pp: paired_memory_ports) {
                auto outer = pp.first;
                auto inner = pp.second;

                /* If we've got a parallel memory then we've got to take the
                 * address bits into account. */
                auto serial_address_match = std::make_shared<rtlir::eqeq_statement>(
                    std::make_shared<rtlir::slice_statement>(
                        portify(outer->address_port()),
                        std::ceil(std::log2(to_compile->depth())) - 1,
                        std::ceil(std::log2(compile_to->depth())) - 0
                    ),
                    std::make_shared<rtlir::literal_statement>(
                        si,
                        std::ceil(std::log2(to_compile->depth())) - std::ceil(std::log2(compile_to->depth()))
                    )
                );
                auto and_address_match = [&](const auto& s) -> rtlir::statement::ptr {
                    if (to_compile->depth() <= compile_to->depth())
                        return s;

                    return std::make_shared<rtlir::and_statement>(s, serial_address_match);
                };
 
                /* FIXME: I don't handle memories with read/write clocks yet. */
                match(
                    std::make_tuple(portify(outer->clock_port()), inner->clock_port()),
                    ds(anyptr, anyptr), [&](const auto& o_c, const auto& i_c) {
                        if (ports_are_already_assigned(o_c, i_c) == false)
                            assign_port(i_c, o_c);
                    },
                    ds(_x, _x), [&](const auto& o_c, const auto& i_c) {
                        std::cerr << "ERROR: Unable to match clock ports on memory\n";
                        std::cerr << "  outer clock port: " << o_c << "\n";
                        std::cerr << "  inner clock port: " << i_c << "\n";
                        abort();
                    }
                );

                /* In order to produce the output of a memory we need to cat
                 * together a bunch of narrower memories, which can only be
                 * done after generating all the memories.  This saves up the
                 * output statements for later. */
                auto generated_valid_output_port_assignment = match(
                    std::make_tuple(portify(outer->output_port()), inner->output_port()),
                    ds(anyptr, anyptr), [&](const auto& o_o, const auto& i_o) {
                        auto w = mktemp(o_o->name(), i_o->width());
                        wire_assign_wire(w, i_o);
                        assign_port(i_o, w);
                        cats.insert(
                            std::make_pair(
                                o_o,
                                std::make_tuple(
                                    w,
                                    parallel_upper - parallel_lower,
                                    outer->address_port()
                                )
                            )
                        );
                        return true;
                    },
                    ds(noneptr, anyptr), [&](const auto& i_o) {
                        /* If the inner memory has an output port but the outer
                         * one doesn't then it's safe to just leave the outer
                         * port floating. */
                        return true;
                    },
                    ds(noneptr, noneptr), [&]() {
                        /* If there's no output ports at all (ie, read-only
                         * port on the memory) then just don't worry about it,
                         * there's nothing to do. */
                        return true;
                    },
                    ds(_x, _x), [&](const auto& o_o, const auto& i_o) {
                        std::cerr << "WARNING: Unable to match output ports on memory\n";
                        std::cerr << "  outer output port: " << o_o << "\n";
                        std::cerr << "  inner output port: " << i_o << "\n";
                        return false;
                    }
                );
                if (generated_valid_output_port_assignment == false)
                    return std::make_shared<rtlir::circuit>(to_compile);

                /* The input port to a memory just needs to happen in parallel,
                 * this does a part select to narrow the memory down. */
                auto generated_valid_input_port_assignment = match(
                    std::make_tuple(portify(outer->input_port()), inner->input_port()),
                    ds(anyptr, anyptr), [&](const auto& o_i, const auto& i_i) {
                        assign(
                            i_i,
                            std::make_shared<rtlir::slice_statement>(
                                o_i,
                                parallel_upper - 1,
                                parallel_lower
                            )
                        );
                        return true;
                    },
                    ds(noneptr, anyptr), [&](const auto& i_i) {
                        /* If the inner memory has an input port but the other
                         * one doesn't then it's safe to just leave the inner
                         * port floating.  This should be handled by the
                         * default value of the write enable, so nothing should
                         * every make it into the memory. */
                        return true;
                    },
                    ds(noneptr, noneptr), [&]() {
                        /* If there's no input ports at all (ie, read-only
                         * port on the memory) then just don't worry about it,
                         * there's nothing to do. */
                        return true;
                    },
                    ds(_x, _x), [&](const auto& o_i, const auto& i_i) {
                        std::cerr << "WARNING: Unable to match input ports on memory\n";
                        std::cerr << "  outer input port: " << o_i << "\n";
                        std::cerr << "  inner input port: " << i_i << "\n";
                        return false;
                    }
                );
                if (generated_valid_input_port_assignment == false)
                    return std::make_shared<rtlir::circuit>(to_compile);

                /* The address port to a memory is just the low-order bits of
                 * the top address. */
                match(
                    std::make_tuple(portify(outer->address_port()), inner->address_port()),
                    ds(anyptr, anyptr), [&](const auto& o_a, const auto& i_a) {
                        assign(
                            i_a,
                            std::make_shared<rtlir::slice_statement>(
                                o_a,
                                std::ceil(std::log2(compile_to->depth())) - 1,
                                0
                            )
                        );
                    },
                    ds(_x, _x), [&](const auto& o_a, const auto& i_a) {
                        std::cerr << "ERROR: Unable to match input ports on memory\n";
                        std::cerr << "  outer input port: " << o_a << "\n";
                        std::cerr << "  inner input port: " << i_a << "\n";
                        abort();
                    }
                );

                /* The bits from the outer memory's write mask that will be
                 * used as the write mask for this inner memory. */
                auto o_mask = match(
                    portify(outer->mask_port()),
                    anyptr, [&](const auto& om) -> rtlir::statement::ptr {
                        if (inner->effective_mask_gran() == inner->bit_width().data()) {
                            return std::make_shared<rtlir::slice_statement>(
                                std::make_shared<rtlir::port_statement>(om),
                                parallel_lower / outer->effective_mask_gran(),
                                parallel_lower / outer->effective_mask_gran()
                            );
                        } else {
                            util::assert(inner->effective_mask_gran() == 1, "I only support single-bit mask ports");
                            return std::make_shared<rtlir::cat_statement>(
                                putil::collection::zip(parallel_lower, parallel_upper,
                                    [&](const auto i) -> rtlir::statement::ptr {
                                        return std::make_shared<rtlir::slice_statement>(
                                            std::make_shared<rtlir::port_statement>(om),
                                            i / outer->effective_mask_gran(),
                                            i / outer->effective_mask_gran()
                                        );
                                    }
                                )
                            );
                        }
                    },
                    noneptr, [&]() -> rtlir::statement::ptr {
                        /* If the outer memory doesn't have a mask then we'll
                         * just fake it by building something we can attach to
                         * the inner mask port that's always enabled. */
                        if (inner->mask_port() == nullptr)
                            return std::make_shared<rtlir::literal_statement>(1, 1);
                        else {
                            return std::make_shared<rtlir::bnot_statement>(
                                std::make_shared<rtlir::literal_statement>(0, inner->mask_port()->width())
                            );
                        }
                    }
                );

                /* The outer memory's write enable port, or a constant 1 if
                 * there isn't a write enable port. */
                auto o_write_enable = match(
                    portify(outer->write_enable_port()),
                    anyptr, [](const auto& owe) -> rtlir::statement::ptr {
                        return std::make_shared<rtlir::port_statement>(owe);
                    },
                    noneptr, [&]() -> rtlir::statement::ptr {
                        /* If there is no input port on the source memory port
                         * then we don't ever want to turn on this write
                         * enable.  Otherwise, we just _always_ turn on the
                         * write enable port on the inner memory. */
                        if (outer->input_port() == nullptr)
                            return std::make_shared<rtlir::literal_statement>(0, 1);
                        else
                            return std::make_shared<rtlir::literal_statement>(1, 1);
                    }
                );

                /* The other memory's chip enable port, or a constant 1 if
                 * there isn't a write enable port. */
                auto o_chip_enable = match(
                    portify(outer->chip_enable_port()),
                    anyptr, [](const auto& oce) -> rtlir::statement::ptr {
                        return std::make_shared<rtlir::port_statement>(oce);
                    },
                    noneptr, []() -> rtlir::statement::ptr {
                        return std::make_shared<rtlir::literal_statement>(1, 1);
                    }
                );

                /* The other memory's chip enable port, or a constant 1 if
                 * there isn't a write enable port. */
                auto o_read_enable = match(
                    portify(outer->read_enable_port()),
                    anyptr, [](const auto& oce) -> rtlir::statement::ptr {
                        return std::make_shared<rtlir::port_statement>(oce);
                    },
                    noneptr, [&]() -> rtlir::statement::ptr {
                        return std::make_shared<rtlir::bnot_statement>(o_write_enable);
                    }
                );


                /* It's safe to ignore read enables, but we pass them through
                 * to the vendor memory if there's a port on there that
                 * implements the read enables. */
                match(
                    inner->read_enable_port(),
                    anyptr, [&](const auto& ire) {
                        assign(ire, and_address_match(o_read_enable));
                    },
                    noneptr, []() {}
                );

                /* This is actually the memory compiler: it figures out how to
                 * implement the outer memory's collection of ports using what
                 * the inner memory has availiable. */
                match(
                    std::make_tuple(inner->mask_port(), inner->write_enable_port(), inner->chip_enable_port()),
                    ds(anyptr, anyptr, anyptr), [&](const auto& i_mask, const auto& i_we, const auto& i_ce) {
                        /* This is the simple option: every port exists. */
                        assign(i_mask, o_mask);
                        assign(i_we, and_address_match(o_write_enable));
                        assign(i_ce, and_address_match(o_chip_enable));
                    },
                    ds(anyptr, anyptr, noneptr), [&](const auto& i_mask, const auto& i_we) {
                        /* If we don't have a chip enable but do have */
                        assign(i_mask, o_mask);
                        assign(
                            i_we,
                            std::make_shared<rtlir::and_statement>(
                                o_chip_enable,
                                and_address_match(o_write_enable)
                            )
                        );
                    },
                    ds(noneptr, anyptr, anyptr), [&](const auto& i_we, const auto& i_ce) {
                        /* If we're expected to provide mask ports without a
                         * memory that actually has them then we can use the
                         * write enable port instead of the mask port. */
                        util::assert(o_mask->width() == 1, "cannot emulate multi-bit mask ports with write enable");
                        assign(
                            i_we,
                            std::make_shared<rtlir::and_statement>(
                                and_address_match(o_write_enable),
                                o_mask
                            )
                        );
                        assign(i_ce, and_address_match(o_chip_enable));
                    },
                    ds(noneptr, noneptr, noneptr), [&]() {
                        /* There's nothing to do here since there aren't any
                         * ports to match up. */
                    },
                    ds(_x, _x, _x), [&](const auto& i_mask, const auto& i_we, const auto& i_ce) {
                        /* If we've gotten here then it means I haven't learned
                         * how to compile these sorts of memories yet. */
                        std::cerr << "ERROR: memory compiler doesn't understand the target memory\n";
                        std::cerr << "  inner write mask: " << i_mask << "\n";
                        std::cerr << "  inner write enable: " << i_we << "\n";
                        std::cerr << "  inner chip enable: " << i_ce << "\n";
                        abort();
                    }
                );
            }

            for (const auto& extra_port: compile_to->extra_ports()) {
                auto as = std::make_shared<rtlir::port_connect_statement>(
                    std::make_shared<rtlir::port>(
                        extra_port->name(),
                        extra_port->width(),
                        rtlir::port_direction::INPUT,
                        rtlir::port_polarity::ACTIVE_HIGH
                    ),
                    std::make_shared<rtlir::literal_statement>(
                        extra_port->width(),
                        extra_port->data()
                    )
                );
                connects.push_back(as);
            }

            auto instance = std::make_shared<rtlir::instance>(
                "mem_" + std::to_string(si) + "_" + std::to_string(pi),
                black_box,
                connects
            );
            instances.push_back(instance);
        }

        auto mktemp = [&](const std::string& base_name, const int& width) {
            auto w = std::make_shared<rtlir::wire>(
                base_name + "_" + std::to_string(si),
                width
            );
            wires.push_back(w);
            return w;
        };

        putil::collection::myfmmw(
            cats,
            [&](const auto& port, const auto& tuples) {
                auto w = mktemp(port->name(), port->width());

                auto cat = std::make_shared<rtlir::cat_statement>(
                    putil::collection::map(
                        tuples,
                        [&](const auto& tuple) -> rtlir::statement::ptr {
                            auto source = std::get<0>(tuple);
                            auto slice_width = std::get<1>(tuple);

                            return std::make_shared<rtlir::slice_statement>(
                                source,
                                slice_width - 1,
                                0
                            );
                        }
                    )
                );

                auto address = [&]() -> rtlir::statement::ptr {
                    if (compile_to->depth() >= to_compile->depth()) {
                        return std::make_shared<rtlir::literal_statement>(0);
                    } else {
                        return std::make_shared<rtlir::slice_statement>(
                            putil::collection::fold(
                                tuples,
                                rtlir::wire::ptr(nullptr),
                                [](const auto& last, const auto& tuple) {
                                    auto wire = std::get<2>(tuple);

                                    if (last == nullptr)
                                        return wire;

                                    util::assert(last->name() == wire->name(), "mismatched cat wire");
                                    return wire;
                                }
                            ),
                            std::ceil(std::log2(to_compile->depth())) - 1,
                            std::ceil(std::log2(compile_to->depth()))
                        );
                    }
                }();

                auto a = std::make_shared<rtlir::connect_statement>(w, cat);
                statements.push_back(a);
                selects.insert(std::make_pair(port, std::make_tuple(w, si, address)));
            }
        );
    }

    putil::collection::myfmmw(
        selects,
        [&](const auto& port, const auto& tuples) {
            auto match = putil::collection::fold(
                tuples,
                std::make_shared<rtlir::literal_statement>(
                    std::make_shared<rtlir::literal>(0, port->width())
                ),
                [&](const auto& last, const auto& tuple) -> rtlir::statement::ptr {
                    auto on_value = std::get<0>(tuple);
                    auto on_address = std::get<1>(tuple);
                    auto address = std::get<2>(tuple);

                    return std::make_shared<rtlir::terop_statement>(
                        std::make_shared<rtlir::eqeq_statement>(
                            std::make_shared<rtlir::literal_statement>(on_address),
                            address
                        ),
                        std::make_shared<rtlir::wire_statement>(on_value),
                        last
                    );
                }
            );

            auto a = std::make_shared<rtlir::connect_statement>(port, match);
            statements.push_back(a);
        }
    );

    auto compiled = std::make_shared<rtlir::module>(
        to_compile->name(),
        wires,
        ports,
        statements,
        instances
    );

    std::cerr << "INFO: compiled " << to_compile->name() << " using " << compile_to->name() << "\n";
    std::cerr << "  Used " << std::to_string(parallel_pairs.size()) << " memories in the width direction\n";

    return std::make_shared<rtlir::circuit>(
        compiled,
        std::vector<rtlir::module::ptr>{black_box, compiled}
    );
}

/* FIXME: The constants 10 and 100 here are hueristics: flops cost 10x an SRAM
 * cell, and SRAM macros cost 100x an SRAM cell. */
size_t memory_cost(const rtlir::module::ptr& module) {
    return match(module,
        someptr<netlist::memory_blackbox>(), [](const auto& mb) -> size_t {
            return mb->width() * mb->depth();
        },
        someptr<netlist::memory_macro>(), [](const auto& mm) -> size_t {
            return mm->width() * mm->depth() * 10;
        },
        someptr<rtlir::module>(), [](const auto& m) -> size_t {
            return putil::collection::fold(
                m->instances(),
                0,
                [](const auto& sum, const auto& m) {
                    return 100 + sum + memory_cost(m->module());
                }
            );
        }
    );
};
