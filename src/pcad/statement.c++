// See LICENSE for details

#include "statement.h++"
using namespace pcad;

std::string unop_to_string(const enum unop_statement::op& op)
{
    switch (op) {
    case unop_statement::op::NOT: return "NOT";
    }
    abort();
}

std::string biop_to_string(const enum biop_statement::op& op)
{
    switch (op) {
    case biop_statement::op::AND:  return "AND";
    case biop_statement::op::OR:   return "OR";
    case biop_statement::op::EQEQ: return "EQEQ";
    case biop_statement::op::LT:   return "LT";
    case biop_statement::op::SUB:  return "SUB";
    case biop_statement::op::LTEQ: return "LTEQ";
    case biop_statement::op::ADD:  return "ADD";
    case biop_statement::op::NEQ:  return "NEQ";
    case biop_statement::op::MUL:  return "MUL";
    case biop_statement::op::XOR:  return "XOR";
    case biop_statement::op::GTEQ: return "GTEQ";
    case biop_statement::op::LSH:  return "LSH";
    case biop_statement::op::RSHL: return "RSHL";
    case biop_statement::op::RSHA: return "RSHA";
    case biop_statement::op::GT:   return "GT";
    }
    abort();
}

void assign_statement::dump(pcad::serialize::json::ofstream& os)
{
    os << pcad::serialize::json::stream_marker::BEGIN_STRUCTURE;
    os << pcad::serialize::json::make_pair("type", "assign");
    os << pcad::serialize::json::make_pair("dest", pcad::serialize::json::stream_marker::NEXT_STRUCTURE);
    _dest->dump(os);
    os << pcad::serialize::json::make_pair("source", pcad::serialize::json::stream_marker::NEXT_STRUCTURE);
    _source->dump(os);
    os << pcad::serialize::json::stream_marker::END_STRUCTURE;
}

void wire_statement::dump(pcad::serialize::json::ofstream& os)
{
    os << pcad::serialize::json::stream_marker::BEGIN_STRUCTURE;
    os << pcad::serialize::json::make_pair("type", "wire");
    os << pcad::serialize::json::make_pair("wire", pcad::serialize::json::stream_marker::NEXT_STRUCTURE);
    _wire->dump(os);
    os << pcad::serialize::json::stream_marker::END_STRUCTURE;
}

void unop_statement::dump(pcad::serialize::json::ofstream& os)
{
    os << pcad::serialize::json::stream_marker::BEGIN_STRUCTURE;
    os << pcad::serialize::json::make_pair("type", "unop");
    os << pcad::serialize::json::make_pair("op", unop_to_string(_op));
    os << pcad::serialize::json::make_pair("statement", pcad::serialize::json::stream_marker::NEXT_STRUCTURE);
    _statement->dump(os);
    os << pcad::serialize::json::stream_marker::END_STRUCTURE;
}

void biop_statement::dump(pcad::serialize::json::ofstream& os)
{
    os << pcad::serialize::json::stream_marker::BEGIN_STRUCTURE;
    os << pcad::serialize::json::make_pair("type", "biop");
    os << pcad::serialize::json::make_pair("op", biop_to_string(_op));
    os << pcad::serialize::json::make_pair("a", pcad::serialize::json::stream_marker::NEXT_STRUCTURE);
    _a->dump(os);
    os << pcad::serialize::json::make_pair("b", pcad::serialize::json::stream_marker::NEXT_STRUCTURE);
    _b->dump(os);
    os << pcad::serialize::json::stream_marker::END_STRUCTURE;
}

void slice_statement::dump(pcad::serialize::json::ofstream& os)
{
    os << pcad::serialize::json::stream_marker::BEGIN_STRUCTURE;
    os << pcad::serialize::json::make_pair("type", "slice");
    os << pcad::serialize::json::make_pair("source", pcad::serialize::json::stream_marker::NEXT_STRUCTURE);
    _source->dump(os);
    os << pcad::serialize::json::make_pair("hi", pcad::serialize::json::stream_marker::NEXT_STRUCTURE);
    _hi->dump(os);
    os << pcad::serialize::json::make_pair("lo", pcad::serialize::json::stream_marker::NEXT_STRUCTURE);
    _lo->dump(os);
    os << pcad::serialize::json::stream_marker::END_STRUCTURE;
}

void cat_statement::dump(pcad::serialize::json::ofstream& os)
{
    os << pcad::serialize::json::stream_marker::BEGIN_STRUCTURE;
    os << pcad::serialize::json::make_pair("type", "cat");
    os << pcad::serialize::json::make_pair("hi", pcad::serialize::json::stream_marker::NEXT_STRUCTURE);
    _hi->dump(os);
    os << pcad::serialize::json::make_pair("lo", pcad::serialize::json::stream_marker::NEXT_STRUCTURE);
    _lo->dump(os);
    os << pcad::serialize::json::stream_marker::END_STRUCTURE;
}

void trop_statement::dump(pcad::serialize::json::ofstream& os)
{
    os << pcad::serialize::json::stream_marker::BEGIN_STRUCTURE;
    os << pcad::serialize::json::make_pair("type", "trinary");
    os << pcad::serialize::json::make_pair("select", pcad::serialize::json::stream_marker::NEXT_STRUCTURE);
    _sel->dump(os);
    os << pcad::serialize::json::make_pair("on true", pcad::serialize::json::stream_marker::NEXT_STRUCTURE);
    _ont->dump(os);
    os << pcad::serialize::json::make_pair("on false", pcad::serialize::json::stream_marker::NEXT_STRUCTURE);
    _onf->dump(os);
    os << pcad::serialize::json::stream_marker::END_STRUCTURE;
}

void rep_statement::dump(pcad::serialize::json::ofstream& os)
{
    os << pcad::serialize::json::stream_marker::BEGIN_STRUCTURE;
    os << pcad::serialize::json::make_pair("type", "rep");
    os << pcad::serialize::json::make_pair("count", pcad::serialize::json::stream_marker::NEXT_STRUCTURE);
    _width->dump(os);
    os << pcad::serialize::json::make_pair("value", pcad::serialize::json::stream_marker::NEXT_STRUCTURE);
    _value->dump(os);
    os << pcad::serialize::json::stream_marker::END_STRUCTURE;
}

void builtin_statement::dump(pcad::serialize::json::ofstream& os)
{
    os << pcad::serialize::json::stream_marker::BEGIN_STRUCTURE;
    os << pcad::serialize::json::make_pair("type", "builtin");
    os << pcad::serialize::json::make_pair("function", _function);
    os << pcad::serialize::json::make_pair("argument", pcad::serialize::json::stream_marker::NEXT_STRUCTURE);
    _arg->dump(os);
    os << pcad::serialize::json::stream_marker::END_STRUCTURE;
}

void always_statement::dump(pcad::serialize::json::ofstream& os)
{
    os << pcad::serialize::json::stream_marker::BEGIN_STRUCTURE;
    os << pcad::serialize::json::make_pair("type", "always");
    os << pcad::serialize::json::make_pair("trigger", pcad::serialize::json::stream_marker::NEXT_STRUCTURE);
    _trigger->dump(os);

    os << pcad::serialize::json::make_pair("body", pcad::serialize::json::stream_marker::BEGIN_ARRAY);
    for (const auto& body: _body)
        body->dump(os);
    os << pcad::serialize::json::stream_marker::END_ARRAY;
    os << pcad::serialize::json::stream_marker::END_STRUCTURE;
}

void posedge_statement::dump(pcad::serialize::json::ofstream& os)
{
    os << pcad::serialize::json::stream_marker::BEGIN_STRUCTURE;
    os << pcad::serialize::json::make_pair("type", "edge");
    os << pcad::serialize::json::make_pair("edge", "posedge");
    os << pcad::serialize::json::make_pair("clock", pcad::serialize::json::stream_marker::NEXT_STRUCTURE);
    _clock->dump(os);
    os << pcad::serialize::json::stream_marker::END_STRUCTURE;
}

void if_statement::dump(pcad::serialize::json::ofstream& os)
{
    os << pcad::serialize::json::stream_marker::BEGIN_STRUCTURE;
    os << pcad::serialize::json::make_pair("type", "if");
    os << pcad::serialize::json::make_pair("cond", pcad::serialize::json::stream_marker::NEXT_STRUCTURE);
    _cond->dump(os);

    os << pcad::serialize::json::make_pair("on true", pcad::serialize::json::stream_marker::BEGIN_ARRAY);
    for (const auto& body: _on_true)
        body->dump(os);
    os << pcad::serialize::json::stream_marker::END_ARRAY;

    os << pcad::serialize::json::make_pair("on false", pcad::serialize::json::stream_marker::BEGIN_ARRAY);
    for (const auto& body: _on_false)
        body->dump(os);
    os << pcad::serialize::json::stream_marker::END_ARRAY;
    os << pcad::serialize::json::stream_marker::END_STRUCTURE;
}
