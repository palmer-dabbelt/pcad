// See LICENSE for details

#include "compile.h++"
#include <pcad/netlist/macro.h++>
#include <pcad/rtlir/module.h++>
#include <pcad/rtlir/instance.h++>
#include <pcad/util/collection.h++>
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

    auto logic = std::vector<rtlir::statement::ptr>();

    auto instances = std::vector<rtlir::instance::ptr>();
    auto cats = std::multimap<rtlir::port::ptr, rtlir::wire::ptr>();
    auto wires = std::vector<rtlir::wire::ptr>();
    auto statements = std::vector<rtlir::statement::ptr>();
    for (auto parallel = 0; parallel < to_compile->width(); parallel += mask_or_macro_width) {
        for (auto serial = 0; serial < to_compile->depth(); serial += compile_to->depth()) {
#ifndef SIMULATIONS_SHOULD_WORK
            if (serial > 0) {
                std::cerr << "INFO: unable to compile " << to_compile->name() << " using " << compile_to->name() << ": depth splitting not supported\n";
                std::cerr << "  " << to_compile->name() << ".depth = " << std::to_string(to_compile->depth()) << "\n";
                std::cerr << "  " << compile_to->name() << ".depth = " << std::to_string(compile_to->depth()) << "\n";
                return std::make_shared<rtlir::circuit>(to_compile);
            }
#endif

            auto pi = parallel / mask_or_macro_width;
            auto si = serial / compile_to->depth();
            auto mi = parallel / mask_width;
            auto connects = std::vector<rtlir::port_connect_statement::ptr>();

            auto assign_port = [&](const rtlir::port::ptr& target, const rtlir::wire::ptr& source) {
                auto as = std::make_shared<rtlir::port_connect_statement>(target, source);
                connects.push_back(as);
                return as;
            };

            auto assign_cat = [&](const rtlir::port::ptr& target, const rtlir::port::ptr& source) {
                auto w = std::make_shared<rtlir::wire>(
                    target->name() + "_" + std::to_string(si) + "_" + std::to_string(pi),
                    source->width()
                );
                wires.push_back(w);

                assign_port(source, w);
                cats.insert(std::make_pair(target, w));

                auto as = std::make_shared<rtlir::connect_statement>(w, source);
                return as;
            };

            auto slice_helper = [&](const rtlir::port::ptr& target, const rtlir::port::ptr& source, int upper, int lower, int width) {
                util::assert(upper >= lower, "backwards slice");

                auto w = std::make_shared<rtlir::wire>(
                    source->name() + "_" + std::to_string(si) + "_" + std::to_string(pi),
                    width
                );
                wires.push_back(w);

                auto ss = std::make_shared<rtlir::slice_statement>(
                    source,
                    std::min(source->width() - 1, upper),
                    lower
                );
                auto ass = std::make_shared<rtlir::connect_statement>(
                    w,
                    ss
                );
                statements.push_back(ass);

                assign_port(target, w);

                return ass;
            };

            auto assign_mask_slice = [&](const rtlir::port::ptr& target, const rtlir::port::ptr& source) {
                return slice_helper(
                    target,
                    source,
                    (pi + 1) * mask_or_macro_width - 1,
                    (pi + 0) * mask_or_macro_width - 0,
                    target->width()
                );
            };

            auto assign_slice = [&](const rtlir::port::ptr& target, const rtlir::port::ptr& source) {
                return slice_helper(
                    target,
                    source,
                    (pi + 1) * target->width() - 1,
                    (pi + 0) * target->width() - 0,
                    target->width()
                );
            };

            auto assign_slice_and = [&](const rtlir::port::ptr& target, const rtlir::wire::ptr& mask, const rtlir::wire::ptr& bit) {
                auto upper = (mi + 1) * target->width() - 1;
                auto lower = (mi + 0) * target->width() - 0;
                util::assert(upper == lower, "only single-bit mask/and is supported");
                util::assert(bit->width() == 1, "only single-bit mask/and is supported");

                auto w = std::make_shared<rtlir::wire>(
                    mask->name() + "_" + std::to_string(si) + "_" + std::to_string(pi) + "_AND_" + bit->name(),
                    upper - lower + 1
                );
                wires.push_back(w);

                auto ss = std::make_shared<rtlir::slice_statement>(
                    mask,
                    upper,
                    lower
                );
                auto ans = std::make_shared<rtlir::and_statement>(
                    ss,
                    bit
                );
                auto asans = std::make_shared<rtlir::connect_statement>(
                    w,
                    ans
                );
                statements.push_back(asans);

                assign_port(target, w);
            };

            auto assign_lower = [&](const rtlir::port::ptr& target, const rtlir::port::ptr& source) {
                return slice_helper(
                    target,
                    source,
                    std::ceil(std::log2(compile_to->depth())) - 1,
                    0,
                    std::ceil(std::log2(compile_to->depth()))
                );
            };
 
            for (const auto& pp: paired_memory_ports) {
                auto outer = pp.first;
                auto inner = pp.second;
                
                auto o_clock = portify(outer->clock_port());
                auto i_clock = inner->clock_port();
                if (o_clock != nullptr && i_clock != nullptr)
                    assign_port(i_clock, o_clock);
                else {
                    std::cerr << "SRAM macro without clock\n";
                    abort();
                    return std::make_shared<rtlir::circuit>(to_compile);
                }

                auto o_output = portify(outer->output_port());
                auto i_output = inner->output_port();
                if (o_output != nullptr && i_output != nullptr)
                    assign_cat(o_output, i_output);
                else if (o_output == nullptr && i_output != nullptr) {
                } else if (o_output == nullptr && i_output == nullptr) {
                } else {
                    std::cerr << "SRAM macro without output: " << to_compile->name() << "\n";
                    abort();
                    return std::make_shared<rtlir::circuit>(to_compile);
                }

                auto o_input = portify(outer->input_port());
                auto i_input = inner->input_port();
                if (o_input != nullptr && i_input != nullptr)
                    assign_mask_slice(i_input, o_input);
                else if (o_input == nullptr && i_input != nullptr) {
                } else if (o_input == nullptr && i_input == nullptr) {
                } else {
                    std::cerr << "SRAM macro without input\n";
                    abort();
                    return std::make_shared<rtlir::circuit>(to_compile);
                }

                auto o_address = portify(outer->address_port());
                auto i_address = inner->address_port();
                if (o_address != nullptr && i_address != nullptr)
                    assign_lower(i_address, o_address);
                else {
                    std::cerr << "SRAM macro without address\n";
                    abort();
                    return std::make_shared<rtlir::circuit>(to_compile);
                }

                auto o_mask = portify(outer->mask_port());
                auto i_mask = inner->mask_port();
                auto o_chip_enable = portify(outer->chip_enable_port());
                auto i_chip_enable = inner->chip_enable_port();
                if (o_mask != nullptr && i_mask != nullptr && o_chip_enable != nullptr && i_chip_enable != nullptr) {
                    /* This is the simple case: everyone has enables and masks,
                     * so just emit the connections. */
                    assign_slice(i_mask, o_mask);
                    assign_port(i_chip_enable, o_chip_enable);
                } else if (o_mask != nullptr && i_mask == nullptr && o_chip_enable == nullptr && i_chip_enable != nullptr) {
                    /* It's possible to implement a mask port using the enable
                     * port, but we only get one bit. */
                    assign_port(i_mask, o_chip_enable);
                } else if (o_mask != nullptr && i_mask == nullptr && o_chip_enable != nullptr && i_chip_enable != nullptr) {
                    /* When the outer macro has a chip enable and a mask, we
                     * can still implement it with the chip enable it's just
                     * that we need to AND together the inputs. */
                    assign_slice_and(i_chip_enable, o_mask, o_chip_enable);
                } else if (o_mask == nullptr && i_mask == nullptr && o_chip_enable != nullptr && i_chip_enable != nullptr) {
                    assign_port(i_chip_enable, o_chip_enable);
                } else {
                    std::cerr << "SRAM macro without mask/chip enable: " << to_compile->name() << "\n";
                    std::cerr << "  " << to_compile->name() << ".chip_enable: " << o_chip_enable << "\n";
                    std::cerr << "  " << compile_to->name() << ".chip_enable: " << i_chip_enable << "\n";
                    std::cerr << "  " << to_compile->name() << ".mask: " << o_mask << "\n";
                    std::cerr << "  " << compile_to->name() << ".mask: " << i_mask << "\n";
                    abort();
                    return std::make_shared<rtlir::circuit>(to_compile);
                }

                auto o_write_enable = portify(outer->write_enable_port());
                auto i_write_enable = inner->write_enable_port();
                if (o_write_enable != nullptr && i_write_enable != nullptr) {
                    assign_port(i_write_enable, o_write_enable);
                } else if (o_write_enable == nullptr && i_write_enable != nullptr) {
                } else {
                    std::cerr << "SRAM macro without write enable\n";
                    abort();
                    return std::make_shared<rtlir::circuit>(to_compile);
                }
            }

            auto instance = std::make_shared<rtlir::instance>(
                "mem_" + std::to_string(si) + "_" + std::to_string(pi),
                black_box,
                connects
            );
            instances.push_back(instance);
        }
    }

    putil::collection::myfmmw(
        cats,
        [&](const rtlir::port::ptr& port, const std::vector<rtlir::wire::ptr>& elements) -> void {
            auto cat = std::make_shared<rtlir::cat_statement>(
                putil::collection::map(
                    elements,
                    [&](const auto& element) -> rtlir::statement::ptr {
                        return std::make_shared<rtlir::slice_statement>(
                            element,
                            mask_or_macro_width - 1,
                            0
                        );
                    }
                )
            );
            auto a = std::make_shared<rtlir::connect_statement>(port, cat);
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
