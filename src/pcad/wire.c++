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

void wire::dump(libjson::ofstream& os) const
{
    os << libjson::stream_marker::BEGIN_STRUCTURE;
    os << libjson::make_pair("name", _name);
    os << libjson::make_pair("width", _width);
    os << libjson::make_pair("depth", _depth);
    os << libjson::stream_marker::END_STRUCTURE;
}
