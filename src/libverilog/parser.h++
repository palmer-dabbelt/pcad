// See LICENSE for details

#ifndef LIBVERILOG__PARSER_HXX
#define LIBVERILOG__PARSER_HXX

#include <pcad/circuit.h++>
#include <memory>
#include "lexer.h++"

namespace libverilog {
    /* A Verilog parser.  This is really only ever going to support the sort of
     * Verilog that FIRRTL can support.  */
    class parser {
    public:
        typedef std::shared_ptr<parser> ptr;

    public:
        static std::vector<pcad::module::ptr> read_file(const std::string& filename);

        static std::vector<pcad::module::ptr> parse_circuit(const std::vector<lexer::token>& tokens);
        static pcad::module::ptr parse_module(const std::vector<lexer::token>& tokens);
    };
}

#endif
