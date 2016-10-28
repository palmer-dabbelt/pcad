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

        for (const auto& to_try: compile_to) {
            auto compiled = compile(to_compile, to_try);

            /* This weights flops as 10x as expensive as an SRAM bit cell, and
             * SRAMs as 100x as expensive as bit cells.  I should really be
             * reading a proper macro file (maybe a lib or lef or db?) that
             * tells me the exact area for all the macros, but I'm lazy.  The
             * 10x is probably OK, but the 100x is garbage. */
            auto compiled_area = compiled == nullptr
                ? (size_t)(-1)
                : memory_cost(compiled->top());
            auto best_area = best_circuit == nullptr
                ? (size_t)(-1)
                : memory_cost(best_circuit->top());

            if (compiled != nullptr)
                std::cerr << "INFO: compiling " << to_compile->name() << " using " << to_try->name() << " costs " << std::to_string(compiled_area) << "\n";

            if (best_area > compiled_area) {
                best_circuit = compiled;
                best_macro = to_try;
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
     * order, but I'm starting with the  */
    if (to_compile->mem_ports().size() != compile_to->mem_ports().size()) {
        std::cerr << "INFO: unable to compile " << to_compile->name() << " using " << compile_to->name() << ": port count must match\n";
        return std::make_shared<rtlir::circuit>(to_compile);
    }
    auto paired_memory_ports = putil::collection::map_zip(
        to_compile->mem_ports(),
        compile_to->mem_ports(),
        [&](const auto& tcp, const auto& ctp) {
            return std::make_pair(tcp, ctp);
        }
    );

    /* There's two ways to split memories based on the depth: either we are
     * limited by the width of the memory, or we're limited by matching the
     * mask of the macro to what we're trying to compile.  */
    auto mask_or_macro_width = putil::collection::fold(
        paired_memory_ports,
        compile_to->width(),
        [](const auto& max, const auto& pair) {
            auto outer = pair.first;
            auto inner = pair.second;

            if (outer->mask_gran().valid() && inner->mask_gran().valid()) {
                auto match_gran = outer->mask_gran().data() % inner->mask_gran().data() ? outer->mask_gran().data() : std::min(outer->mask_gran().data(), inner->mask_gran().data());
                return std::min(max, match_gran);
            }

            if (outer->mask_gran().valid() && !inner->mask_gran().valid()) {
                auto match_gran = std::min(outer->mask_gran().data(), inner->bit_width().data());
                return std::min(max, match_gran);
            }

            auto match_width = std::min(outer->bit_width().data(), inner->bit_width().data());
            return std::min(max, match_width);
        }
    );

    /* When splitting macros based on width, it's possible that we use multiple
     * narrower memories to generate a single mask bit of a larger memory.
     * This width should be used when calculating mask indicies. */
    auto mask_width = putil::collection::fold(
        paired_memory_ports,
        std::max(compile_to->width(), to_compile->width()),
        [](const auto& max, const auto& pair) {
            auto outer = pair.first;
            auto inner = pair.second;

            if (outer->mask_gran().valid() && inner->mask_gran().valid()) {
                auto match_gran = outer->mask_gran().data() % inner->mask_gran().data() ? outer->mask_gran().data() : std::min(outer->mask_gran().data(), inner->mask_gran().data());
                return std::min(max, match_gran);
            }

            if (outer->mask_gran().valid() && !inner->mask_gran().valid())
                return std::min(max, outer->mask_gran().data());

            return max;
        }
    );

    /* This macro compiler is just a horrible mess... */
    auto parallel_pairs = [&](){
        auto out = std::vector<std::pair<int, int>>();
        for (int mask = 0; mask < to_compile->width(); mask += mask_width) {
            auto last_macro = mask;
            for (int macro = mask + mask_or_macro_width; macro < mask + mask_width; macro += mask_or_macro_width) {
                out.push_back(std::make_pair(last_macro, macro));
                last_macro = macro;
            }
            out.push_back(std::make_pair(last_macro, mask + mask_width));
        }
        return out;
    }();
    std::cerr << "INFO: Mapped memory boundaries (mask_or_macro_width=" << std::to_string(mask_or_macro_width) << ", mask_width=" << std::to_string(mask_width) << "):\n";
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
            auto mi = parallel_lower / mask_width;
            auto connects = std::vector<rtlir::port_connect_statement::ptr>();

            auto assign_port = [&](const rtlir::port::ptr& target, const rtlir::wire::ptr& source) {
                auto as = std::make_shared<rtlir::port_connect_statement>(target, source);
                connects.push_back(as);
                return as;
            };

            /* All the helper functions above are garbage, these are the ones that actually work. */
            auto assign = [&](const rtlir::port::ptr& target, const rtlir::statement::ptr& source) {
                auto as = std::make_shared<rtlir::port_connect_statement>(target, source);
                connects.push_back(as);
                return as;
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
                    if (to_compile->depth() == compile_to->depth())
                        return s;

                    return std::make_shared<rtlir::and_statement>(s, serial_address_match);
                };
 
                /* FIXME: I don't handle memories with read/write clocks yet. */
                match(
                    std::make_tuple(portify(outer->clock_port()), inner->clock_port()),
                    ds(anyptr, anyptr), [&](const auto& o_c, const auto& i_c) {
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
                match(
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
                    },
                    ds(noneptr, anyptr), [&](const auto& i_o) {
                        /* If the inner memory has an output port but the outer
                         * one doesn't then it's safe to just leave the outer
                         * port floating. */
                    },
                    ds(_x, _x), [&](const auto& o_o, const auto& i_o) {
                        std::cerr << "ERROR: Unable to match output ports on memory\n";
                        std::cerr << "  outer output port: " << o_o << "\n";
                        std::cerr << "  inner output port: " << i_o << "\n";
                        abort();
                    }
                );

                /* The input port to a memory just needs to happen in parallel,
                 * this does a part select to narrow the memory down. */
                match(
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
                    },
                    ds(noneptr, anyptr), [&](const auto& i_i) {
                        /* If the inner memory has an input port but the other
                         * one doesn't then it's safe to just leave the inner
                         * port floating.  This should be handled by the
                         * default value of the write enable, so nothing should
                         * every make it into the memory. */
                    },
                    ds(_x, _x), [&](const auto& o_i, const auto& i_i) {
                        std::cerr << "ERROR: Unable to match input ports on memory\n";
                        std::cerr << "  outer input port: " << o_i << "\n";
                        std::cerr << "  inner input port: " << i_i << "\n";
                        abort();
                    }
                );

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
                        /* If the outer memory has a mask then select the
                         * correct bits of that mask to go in here. */
                        auto inner_mask_width = inner->mask_port() == nullptr ? 1 : inner->mask_port()->width();
                        util::assert(inner_mask_width > 0, "inner mask width must be greater than zero");
                        auto outer_mask_offset = inner_mask_width * mi;
                        util::assert(outer_mask_offset + inner_mask_width <= om->width(), "outer mask slice too high");

                        return std::make_shared<rtlir::slice_statement>(
                            std::make_shared<rtlir::port_statement>(om),
                            outer_mask_offset + inner_mask_width - 1,
                            outer_mask_offset
                        );
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
                    noneptr, []() -> rtlir::statement::ptr {
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
                    noneptr, []() -> rtlir::statement::ptr {
                        return std::make_shared<rtlir::literal_statement>(1, 1);
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
                    ds(noneptr, anyptr, anyptr), [&](const auto& i_we, const auto& i_ce) {
                        /* If we're expected to provide mask ports without */
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
                    if (compile_to->depth() == to_compile->depth()) {
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
    std::cerr << "  width used for mapping: " << std::to_string(mask_or_macro_width) << "\n";

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
