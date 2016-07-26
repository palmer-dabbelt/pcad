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

void assign_statement::dump(libjson::ofstream& os)
{
    os << libjson::stream_marker::BEGIN_STRUCTURE;
    os << libjson::make_pair("type", "assign");
    os << libjson::make_pair("dest", libjson::stream_marker::NEXT_STRUCTURE);
    _dest->dump(os);
    os << libjson::make_pair("source", libjson::stream_marker::NEXT_STRUCTURE);
    _source->dump(os);
    os << libjson::stream_marker::END_STRUCTURE;
}

void wire_statement::dump(libjson::ofstream& os)
{
    os << libjson::stream_marker::BEGIN_STRUCTURE;
    os << libjson::make_pair("type", "wire");
    os << libjson::make_pair("wire", libjson::stream_marker::NEXT_STRUCTURE);
    _wire->dump(os);
    os << libjson::stream_marker::END_STRUCTURE;
}

void unop_statement::dump(libjson::ofstream& os)
{
    os << libjson::stream_marker::BEGIN_STRUCTURE;
    os << libjson::make_pair("type", "unop");
    os << libjson::make_pair("op", unop_to_string(_op));
    os << libjson::make_pair("statement", libjson::stream_marker::NEXT_STRUCTURE);
    _statement->dump(os);
    os << libjson::stream_marker::END_STRUCTURE;
}

void biop_statement::dump(libjson::ofstream& os)
{
    os << libjson::stream_marker::BEGIN_STRUCTURE;
    os << libjson::make_pair("type", "biop");
    os << libjson::make_pair("op", biop_to_string(_op));
    os << libjson::make_pair("a", libjson::stream_marker::NEXT_STRUCTURE);
    _a->dump(os);
    os << libjson::make_pair("b", libjson::stream_marker::NEXT_STRUCTURE);
    _b->dump(os);
    os << libjson::stream_marker::END_STRUCTURE;
}

void slice_statement::dump(libjson::ofstream& os)
{
    os << libjson::stream_marker::BEGIN_STRUCTURE;
    os << libjson::make_pair("type", "slice");
    os << libjson::make_pair("hi", libjson::stream_marker::NEXT_STRUCTURE);
    _hi->dump(os);
    os << libjson::make_pair("lo", libjson::stream_marker::NEXT_STRUCTURE);
    _lo->dump(os);
    os << libjson::stream_marker::END_STRUCTURE;
}

void cat_statement::dump(libjson::ofstream& os)
{
    os << libjson::stream_marker::BEGIN_STRUCTURE;
    os << libjson::make_pair("type", "cat");
    os << libjson::make_pair("hi", libjson::stream_marker::NEXT_STRUCTURE);
    _hi->dump(os);
    os << libjson::make_pair("lo", libjson::stream_marker::NEXT_STRUCTURE);
    _lo->dump(os);
    os << libjson::stream_marker::END_STRUCTURE;
}

void trop_statement::dump(libjson::ofstream& os)
{
    os << libjson::stream_marker::BEGIN_STRUCTURE;
    os << libjson::make_pair("type", "trinary");
    os << libjson::make_pair("select", libjson::stream_marker::NEXT_STRUCTURE);
    _sel->dump(os);
    os << libjson::make_pair("on true", libjson::stream_marker::NEXT_STRUCTURE);
    _ont->dump(os);
    os << libjson::make_pair("on false", libjson::stream_marker::NEXT_STRUCTURE);
    _onf->dump(os);
    os << libjson::stream_marker::END_STRUCTURE;
}

void rep_statement::dump(libjson::ofstream& os)
{
    os << libjson::stream_marker::BEGIN_STRUCTURE;
    os << libjson::make_pair("type", "rep");
    os << libjson::make_pair("count", libjson::stream_marker::NEXT_STRUCTURE);
    _width->dump(os);
    os << libjson::make_pair("value", libjson::stream_marker::NEXT_STRUCTURE);
    _value->dump(os);
    os << libjson::stream_marker::END_STRUCTURE;
}

void builtin_statement::dump(libjson::ofstream& os)
{
    os << libjson::stream_marker::BEGIN_STRUCTURE;
    os << libjson::make_pair("type", "builtin");
    os << libjson::make_pair("function", _function);
    os << libjson::make_pair("argument", libjson::stream_marker::NEXT_STRUCTURE);
    _arg->dump(os);
    os << libjson::stream_marker::END_STRUCTURE;
}

void always_statement::dump(libjson::ofstream& os)
{
    os << libjson::stream_marker::BEGIN_STRUCTURE;
    os << libjson::make_pair("type", "always");
    os << libjson::make_pair("trigger", libjson::stream_marker::NEXT_STRUCTURE);
    _trigger->dump(os);

    os << libjson::make_pair("body", libjson::stream_marker::BEGIN_ARRAY);
    for (const auto& body: _body)
        body->dump(os);
    os << libjson::stream_marker::END_ARRAY;
    os << libjson::stream_marker::END_STRUCTURE;
}

void posedge_statement::dump(libjson::ofstream& os)
{
    os << libjson::stream_marker::BEGIN_STRUCTURE;
    os << libjson::make_pair("type", "edge");
    os << libjson::make_pair("edge", "posedge");
    os << libjson::make_pair("clock", libjson::stream_marker::NEXT_STRUCTURE);
    _clock->dump(os);
    os << libjson::stream_marker::END_STRUCTURE;
}

void if_statement::dump(libjson::ofstream& os)
{
    os << libjson::stream_marker::BEGIN_STRUCTURE;
    os << libjson::make_pair("type", "if");
    os << libjson::make_pair("cond", libjson::stream_marker::NEXT_STRUCTURE);
    _cond->dump(os);

    os << libjson::make_pair("on true", libjson::stream_marker::BEGIN_ARRAY);
    for (const auto& body: _on_true)
        body->dump(os);
    os << libjson::stream_marker::END_ARRAY;

    os << libjson::make_pair("on false", libjson::stream_marker::BEGIN_ARRAY);
    for (const auto& body: _on_false)
        body->dump(os);
    os << libjson::stream_marker::END_ARRAY;
    os << libjson::stream_marker::END_STRUCTURE;
}
