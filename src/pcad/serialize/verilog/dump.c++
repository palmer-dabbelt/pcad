// See LICENSE for details

#include "dump.h++"
#include <pcad/passes/to_hdlast.h++>
#include <pcad/hdlast/reg.h++>
#include <simple_match/simple_match.hpp>
#include <iostream>

using namespace pcad::hdlast;
using namespace pcad::serialize::verilog;
using namespace simple_match;
using namespace simple_match::placeholders;

static inline std::string v_string(const port_direction& dir)
{
    switch (dir) {
    case port_direction::INPUT:  return "input";
    case port_direction::OUTPUT: return "output";
    }
    abort();
    return "";
}

static inline std::string v_string(const enum unop_statement::op& opcode)
{
    switch (opcode) {
    case unop_statement::op::NOT:  return "~";
    case unop_statement::op::BANG: return "!";
    }
    abort();
    return "";
}

static inline std::string v_string(const enum biop_statement::op& opcode)
{
    switch (opcode) {
    case biop_statement::op::AND:    return "&";
    case biop_statement::op::OR:     return "|";
    case biop_statement::op::EQEQ:   return "==";
    case biop_statement::op::LT:     return "<";
    case biop_statement::op::SUB:    return "-";
    case biop_statement::op::LTEQ:   return "<=";
    case biop_statement::op::ADD:    return "+";
    case biop_statement::op::NEQ:    return "!=";
    case biop_statement::op::MUL:    return "*";
    case biop_statement::op::XOR:    return "^";
    case biop_statement::op::GTEQ:   return ">=";
    case biop_statement::op::LSH:    return "<<";
    case biop_statement::op::RSHL:   return ">>";
    case biop_statement::op::RSHA:   return ">>";
    case biop_statement::op::GT:     return ">";
    case biop_statement::op::OROR:   return "||";
    case biop_statement::op::DIV:    return "/";
    case biop_statement::op::ANDAND: return "&&";
    case biop_statement::op::EQEQEQ: return "===";
    }
    abort();
    return "";
}

static inline std::string width_string(long width)
{
    if (width == 1)
        return "";

    return std::string("[") + std::to_string(width - 1) + ":0]";
}

void pcad::serialize::verilog::dump(std::ofstream& os, const circuit::ptr& circuit)
{
    for (const auto& module: circuit->modules())
        dump(os, module);
}

void pcad::serialize::verilog::dump(std::ofstream& os, const rtlir::circuit::ptr& circuit)
{
    dump(os, passes::to_hdlast(circuit));
}

void pcad::serialize::verilog::dump(std::ofstream& os, const std::vector<rtlir::module::ptr>& modules)
{
    for (const auto& module: modules)
        dump(os, passes::to_hdlast(module));
}

void pcad::serialize::verilog::dump(std::ofstream& os, const module::ptr& module)
{
    os << "module " << module->name() << "(\n";
    for (size_t i = 0; i < module->ports().size(); ++i) {
        auto port = module->ports()[i];
        os << "  "
           << v_string(port->direction())
           << (port->width() == 1 ? "" : " ")
           << width_string(port->width())
           << " "
           << port->name()
           << (i == (module->ports().size() - 1) ? "\n" : ",\n");
    }

    os << ");\n";

    for (const auto& wire: module->body()->vars()) {
        match(wire,
            some<hdlast::reg>(), [&](auto r) {
                os << "  reg"
                   << (r.width() == 1 ? "" : " ")
                   << width_string(r.width())
                   << " "
                   << r.name()
                   << (r.depth() == 1 ? "" : " " + width_string(r.depth()))
                   << ";\n";
            },
            some<hdlast::wire>(), [&](auto w) {
                os << "  wire"
                   << (w.width() == 1 ? "" : " ")
                   << width_string(w.width())
                   << " "
                   << w.name()
                   << (w.depth() == 1 ? "" : " " + width_string(w.depth()))
                   << ";\n";
            }
        );
    }

    for (const auto& instance: module->instances()) {
        os << "  " << instance->module()->name() << " " << instance->name() << "(\n";

        for (size_t i = 0; i < instance->assignments().size(); ++i) {
            auto assignment = instance->assignments()[i];
            os << "    .";
            dump(os, assignment->dest(), "");
            os << "(";
            dump(os, assignment->source(), "");
            os << ")";
            os << (i == (instance->assignments().size() - 1) ? "\n" : ",\n");
        }

        os << "  );\n";
    }

    for (const auto& statement: module->statements())
        dump(os, statement, "  ");

    os << "endmodule\n";
}

void dump_in_always(std::ofstream& os, const statement::ptr& statement, std::string indent)
{
    match(statement,
        some<pcad::hdlast::assign_statement>(), [&](auto s) {
            os << indent;
            dump(os, s.dest(), "");
            os << " <= ";
            dump(os, s.source(), "");
            os << ";\n";
        },
        some<pcad::hdlast::statement>(), [&](auto s) {
            dump(os, statement, indent);
        }
    );
}

void pcad::serialize::verilog::dump(std::ofstream& os, const statement::ptr& statement, std::string indent)
{
    match(statement,
        some<hdlast::assign_statement>(), [&](auto s) {
            os << indent << "assign ";
            dump(os, s.dest(), "");
            os << " = ";
            dump(os, s.source(), "");
            os << ";\n";
        },
        some<hdlast::wire_statement>(), [&](auto s) {
            os << s.wire()->name();
        },
        some<hdlast::unop_statement>(), [&](auto s) {
            os << indent << v_string(s.opcode()) << "(";
            dump(os, s.statement(), "");
            os << ")";
        },
        some<hdlast::biop_statement>(), [&](auto s) {
            os << indent << "(";
            dump(os, s.a(), "");
            os << " " << v_string(s.opcode()) << " ";
            dump(os, s.b(), "");
            os << ")";
        },
        some<hdlast::slice_statement>(), [&](auto s) {
            os << indent;
            dump(os, s.source(), "");
            os << "[";
            dump(os, s.hi(), "");
            os << ":";
            dump(os, s.lo(), "");
            os << "]";
        },
        some<hdlast::index_statement>(), [&](auto s) {
            os << indent;
            dump(os, s.source(), "");
            os << "[";
            dump(os, s.index(), "");
            os << "]";
        },
        some<hdlast::cat_statement>(), [&](auto s) {
            os << indent;
            os << "{";
            dump(os, s.hi(), "");
            os << ", ";
            dump(os, s.lo(), "");
            os << "}";
        },
        some<hdlast::trop_statement>(), [&](auto s) {
            os << indent;
            dump(os, s.sel(), "");
            os << " ? ";
            dump(os, s.ont(), "");
            os << " : ";
            dump(os, s.onf(), "");
        },
        some<hdlast::rep_statement>(), [&](auto s) {
            os << indent;
            os << "{" << s.width()->as_string() << "{";
            dump(os, s.value(), "");
            os << "}}";
        },
        some<hdlast::builtin_statement>(), [&](auto s) {
            os << indent;
            os << s.function() << "(";
            dump(os, s.arg(), "");
            os << ")";
        },
        some<hdlast::always_statement>(), [&](auto s) {
            os << indent;
            os << "always @(";
            dump(os, s.trigger(), "");
            os << ")\n";
            os << indent << "begin\n";
            for (const auto& body: s.body())
                dump_in_always(os, body, indent + "  ");
            os << indent << "end\n";
        },
        some<hdlast::posedge_statement>(), [&](auto s) {
            os << indent;
            os << "posedge " << s.clock()->name();
        },
        some<hdlast::if_statement>(), [&](auto s) {
            os << indent << "if (";
            dump(os, s.cond(), "");
            os << ")\n";
            os << indent << "begin\n";
            for (const auto& statement: s.on_true())
                dump_in_always(os, statement, indent + "  ");
            os << indent << "end\n";
            os << indent << "else\n";
            os << indent << "begin\n";
            for (const auto& statement: s.on_false())
                dump_in_always(os, statement, indent + "  ");
            os << indent << "end\n";
        },
        some<hdlast::statement>(), [&](auto s) {
            std::cerr << "Unknown statement of type " << typeid(statement).name() << std::endl;
            abort();
        },
        none(), [&](){ abort(); }
    );
}
