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

    /* FIXME: I should support mapping to more complicated SRAM port
     * arrangements. */
    if (to_compile->mem_ports().size() != 1 || compile_to->mem_ports().size() != 1) {
        std::cerr << "INFO: unable to compile " << to_compile->name() << " using " << compile_to->name() << ": only single port memories are supported\n";
        return std::make_shared<rtlir::circuit>(to_compile);
    }

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

    auto paired_memory_ports = putil::collection::map(
        to_compile->mem_ports(),
        [&](const auto& tcp) {
            util::assert(compile_to->mem_ports().size() == 1, "only single-port memories supported");
            return std::make_pair(tcp, compile_to->mem_ports()[0]);
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

    auto logic = std::vector<rtlir::statement::ptr>();

    auto instances = std::vector<rtlir::instance::ptr>();
    auto cats = std::multimap<rtlir::port::ptr, rtlir::wire::ptr>();
    auto wires = std::vector<rtlir::wire::ptr>();
    auto statements = std::vector<rtlir::statement::ptr>();
    for (auto parallel = 0; parallel < to_compile->width(); parallel += mask_or_macro_width) {
        for (auto serial = 0; serial < to_compile->depth(); serial += compile_to->depth()) {
            if (serial > 0) {
                std::cerr << "INFO: unable to compile " << to_compile->name() << " using " << compile_to->name() << ": depth splitting not supported\n";
                std::cerr << "  " << to_compile->name() << ".depth = " << std::to_string(to_compile->depth()) << "\n";
                std::cerr << "  " << compile_to->name() << ".depth = " << std::to_string(compile_to->depth()) << "\n";
                return std::make_shared<rtlir::circuit>(to_compile);
            }

            auto pi = parallel / mask_or_macro_width;
            auto si = serial / compile_to->depth();
            auto connects = std::vector<rtlir::port_connect_statement::ptr>();

            auto assign_port = [&](const rtlir::port::ptr& target, const rtlir::wire::ptr& source) {
                auto as = std::make_shared<rtlir::port_connect_statement>(target, source);
                connects.push_back(as);
                return as;
            };

            auto assign_cat = [&](const rtlir::port::ptr& target, const rtlir::port::ptr& source) {
                auto w = std::make_shared<rtlir::wire>(
                    target->name() + "_" + std::to_string(si) + "_" + std::to_string(pi),
                    mask_or_macro_width
                );
                wires.push_back(w);
                assign_port(target, w);
                cats.insert(std::make_pair(target, w));

                auto as = std::make_shared<rtlir::connect_statement>(w, source);
                return as;
            };

            auto slice_helper = [&](const rtlir::port::ptr& target, const rtlir::port::ptr& source, int upper, int lower) {
                auto w = std::make_shared<rtlir::wire>(
                    target->name() + "_" + std::to_string(si) + "_" + std::to_string(pi),
                    upper - lower + 1
                );
                auto ss = std::make_shared<rtlir::slice_statement>(
                    source,
                    upper,
                    lower
                );
                auto ass = std::make_shared<rtlir::connect_statement>(
                    w,
                    ss
                );
                statements.push_back(ass);

                wires.push_back(w);
                assign_port(target, w);

                return ass;
            };

            auto assign_slice = [&](const rtlir::port::ptr& target, const rtlir::port::ptr& source) {
                return slice_helper(
                    target,
                    source,
                    (pi + 1) * source->width() - 1,
                    (pi + 0) * source->width() - 0
                );
            };

            auto assign_lower = [&](const rtlir::port::ptr& target, const rtlir::port::ptr& source) {
                return slice_helper(
                    target,
                    source,
                    std::ceil(std::log2(compile_to->depth())) - 1,
                    0
                );
            };
 
            for (const auto& pp: paired_memory_ports) {
                auto outer = pp.first;
                auto inner = pp.second;
                
                auto o_clock = portify(outer->clock_port());
                auto i_clock = inner->clock_port();
                if (o_clock != nullptr && i_clock != nullptr)
                    assign_port(o_clock, i_clock);
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
                    assign_slice(o_input, i_input);
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
                    assign_lower(o_address, i_address);
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
                    assign_slice(o_mask, i_mask);
                    assign_port(o_chip_enable, i_chip_enable);
                } else if (o_mask != nullptr && i_mask == nullptr && o_chip_enable == nullptr && i_chip_enable != nullptr) {
                    /* It's possible to implement a mask port using the enable
                     * port, but we only get one bit. */
                    assign_port(o_mask, i_chip_enable);
                } else if (o_mask != nullptr && i_mask == nullptr && o_chip_enable != nullptr && i_chip_enable != nullptr) {
                    /* When the outer macro has a chip enable and a mask, we
                     * can still implement it with the chip enable it's just
                     * that we need to AND together the inputs. */
                    assign_port(o_mask, i_chip_enable);
                } else if (o_mask == nullptr && i_mask == nullptr && o_chip_enable != nullptr && i_chip_enable != nullptr) {
                    assign_port(o_chip_enable, i_chip_enable);
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
                    assign_port(o_write_enable, i_write_enable);
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
            auto cat = std::make_shared<rtlir::cat_statement>(elements);
            auto a = std::make_shared<rtlir::port_connect_statement>(port, cat);
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
