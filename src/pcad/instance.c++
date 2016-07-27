// See LICENSE for details

#include "instance.h++"
using namespace pcad;

void instance::dump(libjson::ofstream& os) const
{
    os << libjson::stream_marker::BEGIN_STRUCTURE;
    os << libjson::make_pair("name", _name);
    os << libjson::make_pair("module name", _module->name());
    os << libjson::make_pair("assignments", libjson::stream_marker::BEGIN_ARRAY);
    for (const auto& assignment: _assignments)
        assignment->dump(os);
    os << libjson::stream_marker::END_ARRAY;
    os << libjson::stream_marker::END_STRUCTURE;
}
