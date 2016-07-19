// See LICENSE for details

#include "decoupled.h++"
using namespace pcad;

enum port_direction decoupled::direction(void) const
{
    return _ports[0]->direction();
}
