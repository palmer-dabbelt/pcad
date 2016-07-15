// See LICENSE for details

#include "wire.h++"
using namespace libverilog;

wire::wire(const std::string& name, const long& width)
: _name(name),
  _width(width)
{
}
