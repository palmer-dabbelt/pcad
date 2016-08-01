// See LICENSE for details

#include "open.h++"
#include "serialize/verilog/parser.h++"
#include <iostream>
using namespace pcad;

pcad::hdlast::circuit::ptr pcad::open_circuit(const std::string& name)
{
    try {
        auto v = serialize::verilog::parser::read_file(name);
        if (v.size() > 0)
            return std::make_shared<hdlast::circuit>(v);
    } catch (...) {
        abort();
    }

    std::cerr << "Unable to open circuit by path: " << name << std::endl;
    abort();
    return nullptr;
}
