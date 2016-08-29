// See LICENSE for details

#include "open.h++"
#include "serialize/verilog/parser.h++"
#include <iostream>
using namespace pcad;

pcad::hdlast::circuit::ptr pcad::open_circuit(const std::string& name)
{
    return open_circuit(std::vector<std::string>{name});
}

pcad::hdlast::circuit::ptr pcad::open_circuit(const std::vector<std::string>& names)
{
    try {
        auto v = serialize::verilog::parser::read_files(names);
        if (v.size() > 0)
            return std::make_shared<hdlast::circuit>(v);
    } catch (...) {
        abort();
    }

    std::cerr << "Unable to open circuit by path.  Given files:\n";
    for (const auto& name: names)
        std::cerr << "  " << name << "\n";
    abort();
    return nullptr;
}
