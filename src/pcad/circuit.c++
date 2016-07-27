// See LICENSE for details

#include "circuit.h++"
#include "serialize/verilog/parser.h++"
#include <iostream>
using namespace pcad;

static std::unordered_map<std::string, module::ptr> build_name2module(const std::vector<module::ptr>& modules);

circuit::circuit(const std::string& top, const std::vector<module::ptr>& modules)
: _modules(modules),
  _name2module(build_name2module(modules)),
  _top(this->find_module(top))
{
}

module::ptr circuit::find_module(const std::string& name) const
{
    auto l = _name2module.find(name);
    if (l == _name2module.end())
        return nullptr;

    return l->second;
}

void circuit::dump(pcad::serialize::json::ofstream& os) const
{
    os << pcad::serialize::json::stream_marker::BEGIN_STRUCTURE;
    os << pcad::serialize::json::make_pair("top", _top->name());
    os << pcad::serialize::json::make_pair("modules", pcad::serialize::json::stream_marker::BEGIN_ARRAY);
    for (const auto& module: _modules)
        module->dump(os);
    os << pcad::serialize::json::stream_marker::END_ARRAY;
    os << pcad::serialize::json::stream_marker::END_STRUCTURE;
}

circuit::ptr circuit::read_file(const std::string& filename, const std::string& top)
{
    auto v = serialize::verilog::parser::read_file(filename);
    if (v.size() > 0) {
        return std::make_shared<circuit>(top, v);
    }

    std::cerr << "Unable to parse " << filename << std::endl;
    abort();
}

std::unordered_map<std::string, module::ptr> build_name2module(const std::vector<module::ptr>& modules)
{
    std::unordered_map<std::string, module::ptr> out;

    for (const auto& module: modules)
        out[module->name()] = module;

    return out;
}
