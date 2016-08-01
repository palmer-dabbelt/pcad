// See LICENSE for details

#include "circuit.h++"
#include <iostream>
using namespace pcad::hdlast;

static std::unordered_map<std::string, module::ptr> build_name2module(const std::vector<module::ptr>& modules);

circuit::circuit(const std::vector<module::ptr>& modules)
: _modules(modules),
  _name2module(build_name2module(modules))
{
}

module::ptr circuit::find_module(const std::string& name) const
{
    auto l = _name2module.find(name);
    if (l == _name2module.end())
        return nullptr;

    return l->second;
}

std::unordered_map<std::string, module::ptr> build_name2module(const std::vector<module::ptr>& modules)
{
    std::unordered_map<std::string, module::ptr> out;

    for (const auto& module: modules)
        out[module->name()] = module;

    return out;
}
