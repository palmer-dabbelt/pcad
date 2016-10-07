// See LICENSE for details

#include "compile.h++"
#include <pcad/rtlir/module.h++>
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
            },
            none(), [&]() {
                std::cerr << "Unable to compile macro of type " << typeid(m).name() << "\n";
                abort();
            }
        );
    }
    return out;
}

rtlir::circuit::ptr passes::compile(
    const netlist::memory_macro::ptr& to_compile,
    const std::vector<netlist::memory_macro::ptr>& compile_to __attribute__((unused)))
{
    return std::make_shared<rtlir::circuit>(to_compile);
}
