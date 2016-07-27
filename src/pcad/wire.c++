// See LICENSE for details

#include "wire.h++"
using namespace pcad;

wire::wire(const std::string& name, const long& width)
: _name(name),
  _width(width),
  _depth(1)
{
}

wire::wire(const std::string& name, const long& width, const long& depth)
: _name(name),
  _width(width),
  _depth(depth)
{
}

void wire::dump(pcad::serialize::json::ofstream& os) const
{
    os << pcad::serialize::json::stream_marker::BEGIN_STRUCTURE;
    os << pcad::serialize::json::make_pair("name", _name);
    os << pcad::serialize::json::make_pair("width", _width);
    os << pcad::serialize::json::make_pair("depth", _depth);
    os << pcad::serialize::json::stream_marker::END_STRUCTURE;
}
