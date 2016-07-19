// See LICENSE for details

#include "parser.h++"
#include "lexer.h++"
#include <pcad/module.h++>
#include <pcad/port.h++>
#include <iostream>
using namespace libverilog;
using namespace pcad;

std::vector<module::ptr> parser::read_file(const std::string& filename)
{
    auto l = lexer(filename);
    return parse_circuit(l.tokens());
}

enum class circuit_parser_state {
    TOP,
    MODULE_NAME,
};

std::vector<module::ptr> parser::parse_circuit(const std::vector<lexer::token>& tokens)
{
    auto state = circuit_parser_state::TOP;

    auto modules = std::vector<module::ptr>();
    auto current_module = std::vector<lexer::token>();

    for (const auto& token: tokens) {
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
                current_module = {};
            }
            break;
        }
    }

    return modules;
}

enum class module_parser_state {
    TOP,
    MODULE,
    PORTS,
    PORT_WIDTH_1,
    PORT_WIDTH_2,
    PORT_NAME,
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
    auto ports = std::vector<port::ptr>();

    auto port_direction = option<enum port_direction>();
    auto port_width = option<long>();

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
            state = module_parser_state::PORTS;
            break;

        case module_parser_state::PORTS:
            if (token == "input") {
                port_direction = pcad::port_direction::INPUT;
            } else if (token == "output") {
                port_direction = pcad::port_direction::OUTPUT;
            } else if (token == "[") {
                state = module_parser_state::PORT_WIDTH_1;
            } else if (token == "]") {
                std::cerr << "unknown token ] in module_parser_state::PORTS\n";
                abort();
            } else if (token == ")") {
            } else if (token == "(") {
            } else if (token == ",") {
            } else if (token == ";") {
                state = module_parser_state::DONE;
            } else {
                ports.push_back(std::make_shared<port>(token.str, 1, port_direction.value()));
            }
            break;

        case module_parser_state::PORT_WIDTH_1:
        {
            char *end = NULL;
            auto val = strtol(token.str.c_str(), &end, 0);
            if (end != NULL && *end == '\0') {
                port_width = val + 1;
                state = module_parser_state::PORT_WIDTH_2;
            } else {
                std::cerr << "unknown token " << token.str << " in module_parser_state::PORT_WIDTH_1\n";
                abort();
            }
            
            break;
        }

        case module_parser_state::PORT_WIDTH_2:
        {
            if (token == ":") {
            } else if (token == "0") {
            } else if (token == "]") {
                state = module_parser_state::PORT_NAME;
            } else {
                std::cerr << "unknown token " << token.str << " in module_parser_state::PORT_WIDTH_1\n";
                abort();
            }
            break;
        }

        case module_parser_state::PORT_NAME:
            ports.push_back(std::make_shared<port>(token.str, port_width.value(), port_direction.value()));
            state = module_parser_state::PORTS;
            break;

        case module_parser_state::DONE:
            break;
        }
    }

    return std::make_shared<module>(
        name.value(),
        ports
    );
}
