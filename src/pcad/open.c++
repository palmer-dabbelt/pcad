// See LICENSE for details

#include "open.h++"
#include "serialize/verilog/parser.h++"
#include <iostream>
using namespace pcad;

pcad::hdlast::circuit::ptr pcad::open_circuit(const std::string& path, const std::string& top)
{
    return open_circuit(std::vector<std::string>{path}, top);
}

pcad::hdlast::circuit::ptr pcad::open_circuit(const std::vector<std::string>& names, const std::string& top)
{
    try {
        auto v = serialize::verilog::parser::read_files(names);
        if (v.size() > 0)
            return std::make_shared<hdlast::circuit>(v, top);
    } catch (...) {
        abort();
    }

    std::cerr << "Unable to open circuit by path.  Given files:\n";
    for (const auto& name: names)
        std::cerr << "  " << name << "\n";
    abort();
    return nullptr;
}
