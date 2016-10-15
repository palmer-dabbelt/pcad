// See LICENSE for details

#include "compile.h++"
#include <pcad/netlist/macro.h++>
#include <pcad/rtlir/module.h++>
#include <pcad/rtlir/instance.h++>
#include <pcad/util/collection.h++>
#include <simple_match/simple_match.hpp>
using namespace pcad;
using namespace simple_match;
using namespace simple_match::placeholders;

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
    static const auto count_flops = [](const auto& module) {
        return 1;
    };
    static const auto count_cells = [](const auto& module) {
        return 1;
    };
    static const auto count_instances = [](const auto& module) {
        return 1;
    };

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
                : count_instances(compiled->top()) * 100 + count_flops(compiled->top()) * 10 + count_cells(compiled->top());
            auto best_area = best_circuit == nullptr
                ? (size_t)(-1)
                : count_instances(best_circuit->top()) * 100 + count_flops(best_circuit->top()) * 10 + count_cells(best_circuit->top());

            if (best_area > compiled_area) {
                best_circuit = compiled;
                best_macro = to_try;
            }
        }

        return best_macro;
    }();

    return compile(to_compile, vmacro);
}

rtlir::circuit::ptr passes::compile(
    const netlist::memory_macro::ptr& to_compile,
    const netlist::memory_macro::ptr& compile_to)
{
    if (compile_to == nullptr)
        return std::make_shared<rtlir::circuit>(to_compile);

    /* FIXME: I should compile to larger memories than the desired macro, but
     * I'm lazy. */
    if (compile_to->width() > to_compile->width() || compile_to->depth() > to_compile->depth())
        return std::make_shared<rtlir::circuit>(to_compile);

    /* FIXME: I should support mapping to non-exact sizes, but I'm lazy. */
    if (to_compile->width() % compile_to->width() != 0 || to_compile->depth() % compile_to->depth() != 0)
        return std::make_shared<rtlir::circuit>(to_compile);

    auto black_box = std::make_shared<netlist::memory_blackbox>(compile_to);

    auto instances = std::vector<rtlir::instance::ptr>();
    for (auto parallel = 0; parallel < to_compile->width(); parallel += compile_to->width()) {
        instances.push_back(
            std::make_shared<rtlir::instance>(
                "sram_0_" + std::to_string(parallel / compile_to->width()),
                black_box
            )
        );
    }

    auto compiled = std::make_shared<rtlir::module>(
        to_compile->name(),
        instances
    );

    return std::make_shared<rtlir::circuit>(
        compiled,
        std::vector<rtlir::module::ptr>{black_box, compiled}
    );
}
