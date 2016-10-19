// See LICENSE for details

#include "compile.h++"
#include <pcad/netlist/macro.h++>
#include <pcad/rtlir/module.h++>
#include <pcad/rtlir/instance.h++>
#include <pcad/util/collection.h++>
#include <simple_match/simple_match.hpp>
#include <pcad/util/assert.h++>
#include <map>
#include <unordered_map>
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

    /* FIXME: I should support mapping to more complicated SRAM port
     * arrangements. */
    if (to_compile->mem_ports().size() != 1 || compile_to->mem_ports().size() != 1)
        return std::make_shared<rtlir::circuit>(to_compile);

    auto black_box = std::make_shared<netlist::memory_blackbox>(compile_to);

    auto name2port = std::unordered_map<std::string, rtlir::port::ptr>();
    auto ports = std::vector<rtlir::port::ptr>();
    auto portify = [&](const rtlir::port::ptr& mmp) {
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
            return std::make_pair(tcp, compile_to->mem_ports()[0]);
        }
    );

    auto logic = std::vector<rtlir::statement::ptr>();

    auto instances = std::vector<rtlir::instance::ptr>();
    auto cats = std::multimap<rtlir::port::ptr, rtlir::wire::ptr>();
    auto wires = std::vector<rtlir::wire::ptr>();
    auto statements = std::vector<rtlir::statement::ptr>();
    for (auto parallel = 0; parallel < to_compile->width(); parallel += compile_to->width()) {
        for (auto serial = 0; serial < to_compile->depth(); serial += compile_to->depth()) {
            util::assert(serial == 0, "I don't support memory depth splitting yet");

            auto pi = parallel / compile_to->width();
            auto si = serial / compile_to->depth();
            auto connects = std::vector<rtlir::connect_statement::ptr>();

            auto assign_always = [&](const rtlir::wire::ptr& target, const rtlir::wire::ptr& source) {
                auto as = std::make_shared<rtlir::connect_statement>(target, source);
                connects.push_back(as);
                return as;
            };

            auto assign_cat = [&](const rtlir::port::ptr& target, const rtlir::port::ptr& source) {
                auto w = std::make_shared<rtlir::wire>(
                    target->name() + "_" + std::to_string(si) + "_" + std::to_string(pi),
                    compile_to->width()
                );
                wires.push_back(w);
                cats.insert(std::make_pair(target, w));
                return assign_always(source, w);
            };

            auto assign_slice = [&](const rtlir::port::ptr& target, const rtlir::port::ptr& source) {
                auto w = std::make_shared<rtlir::wire>(
                    target->name() + "_" + std::to_string(si) + "_" + std::to_string(pi),
                    compile_to->width()
                );
                auto ss = std::make_shared<rtlir::slice_statement>(
                    source,
                    parallel + compile_to->width() - 1,
                    parallel
                );
                auto ass = std::make_shared<rtlir::connect_statement>(
                    w,
                    ss
                );
                statements.push_back(ass);

                wires.push_back(w);
                return assign_always(source, w);
            };
 
            for (const auto& pp: paired_memory_ports) {
                auto outer = pp.first;
                auto inner = pp.second;
                
                auto o_clock = portify(outer->clock_port());
                auto i_clock = portify(inner->clock_port());
                assign_always(o_clock, i_clock);

                auto o_output = portify(outer->output_port());
                auto i_output = portify(inner->output_port());
                assign_cat(o_output, i_output);

                auto o_input = portify(outer->input_port());
                auto i_input = portify(inner->input_port());
                assign_slice(o_input, i_input);
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

    return std::make_shared<rtlir::circuit>(
        compiled,
        std::vector<rtlir::module::ptr>{black_box, compiled}
    );
}
