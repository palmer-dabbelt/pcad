// See LICENSE for details

#include "parser.h++"
#include "lexer.h++"
#include "module.h++"
#include <iostream>
using namespace libverilog;

circuit::ptr parser::read_file(const std::string& filename)
{
    auto l = lexer(filename);
    return parse_circuit(l.tokens());
}

enum class circuit_parser_state {
    TOP,
    MODULE_NAME,
};

circuit::ptr parser::parse_circuit(const std::vector<lexer::token>& tokens)
{
    auto state = circuit_parser_state::TOP;

    auto modules = std::vector<module::ptr>();
    auto current_module = std::vector<lexer::token>();

    for (const auto& token: tokens) {
        std::cerr << "token: " << token.str << "\n";

        switch (state) {
        case circuit_parser_state::TOP:
            if (token == "module") {
                state = circuit_parser_state::MODULE_NAME;
                current_module.push_back(token);
            } else {
                std::cerr << "parser/TOP: " << token.str << "\n";
                abort();
            }
            break;

        case circuit_parser_state::MODULE_NAME:
            current_module.push_back(token);
            if (token == "endmodule") {
                state = circuit_parser_state::TOP;
                modules.push_back(parse_module(current_module));
            }
            break;
        }
    }

    return std::make_shared<circuit>(modules);
}

enum class module_parser_state {
    TOP,
    MODULE,
    DONE,
};

template<typename T>
class option {
public:
    bool _valid;
    T _value;

    option(void)
    : _valid(false)
    {}

    option(const T& value)
    : _valid(true),
      _value(value)
    {}

    const T& value(void) const {
        if (_valid == false)
            abort();

        return _value;
    }
};

module::ptr parser::parse_module(const std::vector<lexer::token>& tokens)
{
    auto state = module_parser_state::TOP;

    auto name = option<std::string>(); 

    for (const auto& token: tokens) {
        switch (state) {
        case module_parser_state::TOP:
            if (token == "module")
                state = module_parser_state::MODULE;
            else {
                std::cerr << "module didn't start with module, but instead " << token.str << "\n";
                abort();
            }
            break;

        case module_parser_state::MODULE:
            name = token.str;
            state = module_parser_state::DONE;
            break;

        case module_parser_state::DONE:
            break;
        }
    }

    return std::make_shared<module>(
        name.value()
    );
}
