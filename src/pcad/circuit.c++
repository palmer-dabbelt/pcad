// See LICENSE for details

#include "circuit.h++"
#include <libverilog/parser.h++>
#include <iostream>
using namespace pcad;

static std::unordered_map<std::string, libverilog::module::ptr> build_name2module(const std::vector<libverilog::module::ptr>& modules);

circuit::circuit(const std::string& top, const libverilog::circuit::ptr& v)
: _v(v),
  _name2module(build_name2module(v->modules())),
  _top(this->module(top))
{
}

libverilog::module::ptr circuit::module(const std::string& name) const
{
    auto l = _name2module.find(name);
    if (l == _name2module.end())
        return nullptr;

    return l->second;
}

circuit::ptr circuit::read_file(const std::string& filename, const std::string& top)
{
    auto v = libverilog::parser::read_file(filename);
    if (v != nullptr) {
        return std::make_shared<circuit>(top, v);
    }

    std::cerr << "Unable to parse " << filename << std::endl;
    abort();
}

std::unordered_map<std::string, libverilog::module::ptr> build_name2module(const std::vector<libverilog::module::ptr>& modules)
{
    std::unordered_map<std::string, libverilog::module::ptr> out;

    for (const auto& module: modules)
        out[module->name()] = module;

    return out;
}
