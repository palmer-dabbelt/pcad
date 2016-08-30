// See LICENSE for details

#include "strip.h++"
#include <set>
using namespace pcad;

hdlast::circuit::ptr passes::strip(const hdlast::circuit::ptr& big)
{
    auto used_modules = std::set<std::string>();
    for (const auto& module: big->modules())
        for (const auto& instance: module->instances())
            used_modules.insert(instance->module()->name());
    used_modules.insert(big->top()->name());

    auto small_modules = std::vector<hdlast::module::ptr>();
    for (const auto& module_name: used_modules)
        small_modules.push_back(big->find_module(module_name));

    return std::make_shared<hdlast::circuit>(small_modules, big->top());
}
