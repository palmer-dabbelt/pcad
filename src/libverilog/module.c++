// See LICENSE for details

#include "module.h++"
using namespace libverilog;

module::module(const decltype(_name)& name,
               const decltype(_ports)& ports)
: _name(name),
  _ports(ports)
{
}
