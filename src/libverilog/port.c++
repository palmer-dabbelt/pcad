// See LICENSE for details

#include "port.h++"
using namespace libverilog;

port::port(const std::string& name, const long& width, const enum port_direction& dir)
: wire(name, width),
  _direction(dir)
{
}

std::string libverilog::to_string(const enum port_direction& d)
{
    switch (d) {
    case port_direction::INPUT: return "input";
    case port_direction::OUTPUT: return "output";
    }

    abort();
    return "";
}
