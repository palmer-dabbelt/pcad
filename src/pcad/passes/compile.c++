// See LICENSE for details

#include "compile.h++"
#include <pcad/rtlir/module.h++>
using namespace pcad;

std::vector<rtlir::circuit::ptr> passes::compile(
    const std::vector<netlist::macro::ptr>& to_compile,
    const std::vector<netlist::macro::ptr>& compile_to __attribute__((unused)))
{
    auto out = std::vector<rtlir::circuit::ptr>();
    for (const auto& m: to_compile)
        out.push_back(std::make_shared<rtlir::circuit>(m));
    return out;
}
