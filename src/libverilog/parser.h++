// See LICENSE for details

#ifndef LIBVERILOG__PARSER_HXX
#define LIBVERILOG__PARSER_HXX

#include <pcad/circuit.h++>
#include <pcad/instance.h++>
#include <pcad/literal.h++>
#include <pcad/statement.h++>
#include <memory>
#include <unordered_map>
#include "lexer.h++"

namespace libverilog {
    /* A Verilog parser.  This is really only ever going to support the sort of
     * Verilog that FIRRTL can support.  */
    class parser {
    public:
        typedef std::shared_ptr<parser> ptr;

    public:
        static std::vector<pcad::module::ptr>
        read_file(const std::string& filename);

    private:
        static std::vector<pcad::module::ptr>
        parse_circuit(const std::vector<lexer::token>& tokens);

        static pcad::module::ptr
        parse_module(const std::vector<lexer::token>& tokens,
                     const std::unordered_map<std::string, pcad::module::ptr>& module_lookup);

        static pcad::statement::ptr
        parse_statement(const std::vector<lexer::token>& tokens,
                        const pcad::scope::ptr& scope);

        static std::vector<pcad::statement::ptr>
        parse_statements(const std::vector<lexer::token>& tokens,
                         const pcad::scope::ptr& scope);

        static pcad::wire::ptr parse_wire(const lexer::token& token,
                                          const pcad::scope::ptr& scope);

        static pcad::literal::ptr parse_literal(const lexer::token& token);

        static pcad::instance::ptr
        parse_instance(const std::vector<lexer::token>& tokens,
                       const std::unordered_map<std::string, pcad::module::ptr>& module_lookup,
                       const pcad::scope::ptr& scope);
    };
}

#endif
