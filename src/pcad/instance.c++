// See LICENSE for details

#include "instance.h++"
using namespace pcad;

void instance::dump(pcad::serialize::json::ofstream& os) const
{
    os << pcad::serialize::json::stream_marker::BEGIN_STRUCTURE;
    os << pcad::serialize::json::make_pair("name", _name);
    os << pcad::serialize::json::make_pair("module name", _module->name());
    os << pcad::serialize::json::make_pair("assignments", pcad::serialize::json::stream_marker::BEGIN_ARRAY);
    for (const auto& assignment: _assignments)
        assignment->dump(os);
    os << pcad::serialize::json::stream_marker::END_ARRAY;
    os << pcad::serialize::json::stream_marker::END_STRUCTURE;
}
