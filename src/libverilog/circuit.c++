// See LICENSE for details

#include "circuit.h++"
using namespace libverilog;

circuit::circuit(const std::vector<pcad::module::ptr>& modules)
: _modules(modules)
{
}
