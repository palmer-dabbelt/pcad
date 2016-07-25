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
