// See LICENSE for details

#include "dump.h++"
#include <simple_match/simple_match.hpp>
#include <iostream>

using namespace pcad::hdlast;
using namespace pcad::serialize::json;
using namespace simple_match;
using namespace simple_match::placeholders;

void pcad::serialize::json::dump(ofstream& os, const circuit::ptr& circuit, const std::string& top)
{
    os << stream_marker::BEGIN_STRUCTURE;
    os << make_pair("top", top);
    os << make_pair("modules", stream_marker::BEGIN_ARRAY);
    for (const auto& module: circuit->modules())
        dump(os, module);
    os << stream_marker::END_ARRAY;
    os << stream_marker::END_STRUCTURE;
}

void pcad::serialize::json::dump(ofstream& os, const module::ptr& module)
{
    os << stream_marker::BEGIN_STRUCTURE;
    os << make_pair("name", module->name());
    os << make_pair("ports", stream_marker::BEGIN_ARRAY);
    for (const auto& port: module->ports())
        dump(os, port);
    os << stream_marker::END_ARRAY;
    os << make_pair("wires", stream_marker::BEGIN_ARRAY);
    for (const auto& wire: module->body()->vars())
        dump(os, wire);
    os << stream_marker::END_ARRAY;
    os << make_pair("statements", stream_marker::BEGIN_ARRAY);
    for (const auto& statement: module->statements())
        dump(os, statement);
    os << stream_marker::END_ARRAY;
    os << make_pair("instances", stream_marker::BEGIN_ARRAY);
    for (const auto& instance: module->instances())
        dump(os, instance);
    os << stream_marker::END_ARRAY;
    os << stream_marker::END_STRUCTURE;
}

void pcad::serialize::json::dump(ofstream& os, const wire::ptr& wire)
{
    os << stream_marker::BEGIN_STRUCTURE;

    match(wire,
        some<hdlast::port>(), [&](auto p){
            os << make_pair("name", p.name());
            os << make_pair("width", p.width());
            os << make_pair("depth", p.depth());
            os << make_pair("direction", to_string(p.direction()));
        },
        some<hdlast::wire>(), [&](auto w){
            os << make_pair("name", w.name());
            os << make_pair("width", w.width());
            os << make_pair("depth", w.depth());
        },
        none(), [&](){ abort(); }
    );

    os << stream_marker::END_STRUCTURE;
}

void pcad::serialize::json::dump(ofstream& os, const statement::ptr& statement)
{
    os << stream_marker::BEGIN_STRUCTURE;

    match(statement,
        some<hdlast::assign_statement>(), [&](auto s) {
            os << make_pair("type", "assign");
            os << make_pair("dest", stream_marker::NEXT_STRUCTURE);
            dump(os, s.dest());
            os << make_pair("source", stream_marker::NEXT_STRUCTURE);
            dump(os, s.source());
        },
        some<hdlast::wire_statement>(), [&](auto s) {
            os << make_pair("type", "wire");
            os << make_pair("wire", stream_marker::NEXT_STRUCTURE);
            dump(os, s.wire());
        },
        some<hdlast::unop_statement>(), [&](auto s) {
            os << make_pair("type", "unop");
        },
        some<hdlast::biop_statement>(), [&](auto s) {
            os << make_pair("type", "biop");
            os << make_pair("op", s.opstr());
            os << make_pair("a", stream_marker::NEXT_STRUCTURE);
            dump(os, s.a());
            os << make_pair("b", stream_marker::NEXT_STRUCTURE);
            dump(os, s.b());
        },
        some<hdlast::slice_statement>(), [&](auto s) {
            os << make_pair("type", "slice");
            os << make_pair("source", stream_marker::NEXT_STRUCTURE);
            dump(os, s.source());
            os << make_pair("hi", stream_marker::NEXT_STRUCTURE);
            dump(os, s.hi());
            os << make_pair("lo", stream_marker::NEXT_STRUCTURE);
            dump(os, s.lo());
        },
        some<hdlast::index_statement>(), [&](auto s) {
            os << make_pair("type", "index");
            os << make_pair("source", stream_marker::NEXT_STRUCTURE);
            dump(os, s.source());
            os << make_pair("index", stream_marker::NEXT_STRUCTURE);
            dump(os, s.index());
        },
        some<hdlast::cat_statement>(), [&](auto s) {
            os << make_pair("type", "cat");
        },
        some<hdlast::trop_statement>(), [&](auto s) {
            os << make_pair("type", "trop");
        },
        some<hdlast::rep_statement>(), [&](auto s) {
            os << make_pair("type", "rep");
        },
        some<hdlast::builtin_statement>(), [&](auto s) {
            os << make_pair("type", "builtin");
        },
        some<hdlast::always_statement>(), [&](auto s) {
            os << make_pair("type", "always");
            os << make_pair("trigger", stream_marker::NEXT_STRUCTURE);
            dump(os, s.trigger());
            os << make_pair("body", stream_marker::BEGIN_ARRAY);
            for (const auto& bs: s.body())
                dump(os, bs);
            os << stream_marker::END_ARRAY;
        },
        some<hdlast::posedge_statement>(), [&](auto s) {
            os << make_pair("type", "edge");
            os << make_pair("edge", "posedge");
            os << make_pair("clock", stream_marker::NEXT_STRUCTURE);
            dump(os, s.clock());
        },
        some<hdlast::if_statement>(), [&](auto s) {
            os << make_pair("type", "if");
        },
        some<hdlast::statement>(), [&](auto s){
            std::cerr << "Unknown statement of type " << typeid(s).name() << std::endl;
            abort();
        },
        none(), [&](){ abort(); }
    );

    os << stream_marker::END_STRUCTURE;
}

void pcad::serialize::json::dump(ofstream& os, const instance::ptr& instance)
{
    os << stream_marker::BEGIN_STRUCTURE;

    os << make_pair("name", instance->name());
    os << make_pair("module name", instance->module()->name());
    os << make_pair("assignments", stream_marker::BEGIN_ARRAY);
    for (const auto& a: instance->assignments())
        dump(os, a);
    os << stream_marker::END_ARRAY;

    os << stream_marker::END_STRUCTURE;
}
