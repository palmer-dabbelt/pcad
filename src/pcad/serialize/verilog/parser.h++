// See LICENSE for details

#ifndef PCAD__SERIALIZE__VERILOG__PARSER_HXX
#define PCAD__SERIALIZE__VERILOG__PARSER_HXX

#include <pcad/hdlast/circuit.h++>
#include <pcad/hdlast/instance.h++>
#include <pcad/hdlast/literal.h++>
#include <pcad/hdlast/statement.h++>
#include <memory>
#include <unordered_map>
#include "lexer.h++"

namespace pcad {
    namespace serialize {
        namespace verilog {
            /* A Verilog parser.  This is really only ever going to support the sort of
             * Verilog that FIRRTL can support.  */
            class parser {
            public:
                typedef std::shared_ptr<parser> ptr;

            public:
                static std::vector<pcad::hdlast::module::ptr>
                read_file(const std::string& filename);

                static std::vector<pcad::hdlast::module::ptr>
                read_files(const std::vector<std::string>& filenames);

            private:
                static std::vector<pcad::hdlast::module::ptr>
                parse_circuit(const std::vector<lexer::token>& tokens);

                static pcad::hdlast::module::ptr
                parse_module(const std::vector<lexer::token>& tokens,
                             const std::unordered_map<std::string, pcad::hdlast::module::ptr>& module_lookup);

                static pcad::hdlast::statement::ptr
                parse_statement(const std::vector<lexer::token>& tokens,
                                const pcad::hdlast::scope::ptr& scope);

                static std::vector<pcad::hdlast::statement::ptr>
                parse_statements(const std::vector<lexer::token>& tokens,
                                 const pcad::hdlast::scope::ptr& scope);

                static pcad::hdlast::wire::ptr parse_wire(const lexer::token& token,
                                                  const pcad::hdlast::scope::ptr& scope);

                static pcad::hdlast::literal::ptr parse_literal(const lexer::token& token);

                static pcad::hdlast::instance::ptr
                parse_instance(const std::vector<lexer::token>& tokens,
                               const std::unordered_map<std::string, pcad::hdlast::module::ptr>& module_lookup,
                               const pcad::hdlast::scope::ptr& scope);
            };
        }
    }
}

#endif
