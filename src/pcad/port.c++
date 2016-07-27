// See LICENSE for details

#include "port.h++"
using namespace pcad;

port::port(const std::string& name, const long& width, const enum port_direction& dir)
: wire(name, width),
  _direction(dir)
{
}

void port::dump(pcad::serialize::json::ofstream& os) const
{
    os << pcad::serialize::json::stream_marker::BEGIN_STRUCTURE;
    os << pcad::serialize::json::make_pair("name", name());
    os << pcad::serialize::json::make_pair("width", width());
    os << pcad::serialize::json::make_pair("depth", depth());
    os << pcad::serialize::json::make_pair("direction", to_string(_direction));
    os << pcad::serialize::json::stream_marker::END_STRUCTURE;
}

std::string pcad::to_string(const enum port_direction& d)
{
    switch (d) {
    case port_direction::INPUT: return "input";
    case port_direction::OUTPUT: return "output";
    }

    abort();
    return "";
}
