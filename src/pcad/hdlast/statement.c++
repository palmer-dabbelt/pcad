// See LICENSE for details

#include "statement.h++"
using namespace pcad::hdlast;

std::string unop_to_string(const enum unop_statement::op& op)
{
    switch (op) {
    case unop_statement::op::NOT:  return "NOT";
    case unop_statement::op::BANG: return "BANG";
    }
    abort();
}

std::string biop_statement::opstr(const enum biop_statement::op& op)
{
    switch (op) {
    case biop_statement::op::AND:    return "AND";
    case biop_statement::op::OR:     return "OR";
    case biop_statement::op::EQEQ:   return "EQEQ";
    case biop_statement::op::LT:     return "LT";
    case biop_statement::op::SUB:    return "SUB";
    case biop_statement::op::LTEQ:   return "LTEQ";
    case biop_statement::op::ADD:    return "ADD";
    case biop_statement::op::NEQ:    return "NEQ";
    case biop_statement::op::MUL:    return "MUL";
    case biop_statement::op::XOR:    return "XOR";
    case biop_statement::op::GTEQ:   return "GTEQ";
    case biop_statement::op::LSH:    return "LSH";
    case biop_statement::op::RSHL:   return "RSHL";
    case biop_statement::op::RSHA:   return "RSHA";
    case biop_statement::op::GT:     return "GT";
    case biop_statement::op::OROR:   return "OROR";
    case biop_statement::op::DIV:    return "DIV";
    case biop_statement::op::ANDAND: return "ANDAND";
    case biop_statement::op::EQEQEQ: return "EQEQEQ";
    }
    abort();
}
