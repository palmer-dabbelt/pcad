// See LICENSE for details

#include "parser.h++"
#include "lexer.h++"
#include <pcad/instance.h++>
#include <pcad/module.h++>
#include <pcad/port.h++>
#include <pcad/builtin_wire.h++>
#include <pcad/scope.h++>
#include <pcad/statement.h++>
#include <iostream>
#include <stack>
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
    auto module_lookup = std::unordered_map<std::string, pcad::module::ptr>();

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
                auto module = parse_module(current_module, module_lookup);
                modules.push_back(module);
                module_lookup[module->name()] = module;
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
    BODY,
    ASSIGN_NAME,
    ASSIGN_OP,
    ASSIGN_STATEMENT,
    INITIAL_START,
    INITIAL_STATEMENT,
    INITIAL_BLOCK,
    WIRE_NAME_OR_WIDTH,
    WIRE_WIDTH,
    WIRE_NAME,
    WIRE_DEPTH,
    INSTANCE,
    ALWAYS_START,
    ALWAYS_AT,
    ALWAYS_STATEMENT,
    ALWAYS_BLOCK,
    ALWAYS_CASE,
    ALWAYS_BODY,
    PP_IFDEF,
    DONE,
};

std::string to_string(enum module_parser_state state) {
    switch (state) {
    case module_parser_state::TOP: return "TOP";
    case module_parser_state::MODULE: return "MODULE";
    case module_parser_state::PORTS: return "PORTS";
    case module_parser_state::PORT_WIDTH_1: return "PORT_WIDTH_1";
    case module_parser_state::PORT_WIDTH_2: return "PORT_WIDTH_2";
    case module_parser_state::PORT_NAME: return "PORT_NAME";
    case module_parser_state::BODY: return "BODY";
    case module_parser_state::ASSIGN_NAME: return "ASSIGN_NAME";
    case module_parser_state::ASSIGN_OP: return "ASSIGN_OP";
    case module_parser_state::ASSIGN_STATEMENT: return "ASSIGN_STATEMENT";
    case module_parser_state::INITIAL_START: return "INITIAL_START";
    case module_parser_state::INITIAL_STATEMENT: return "INITIAL_STATEMENT";
    case module_parser_state::INITIAL_BLOCK: return "INITIAL_BLOCK";
    case module_parser_state::WIRE_NAME_OR_WIDTH: return "WIRE_NAME_OR_WIDTH";
    case module_parser_state::WIRE_WIDTH: return "WIRE_WIDTH";
    case module_parser_state::WIRE_NAME: return "WIRE_NAME";
    case module_parser_state::WIRE_DEPTH: return "WIRE_DEPTH";
    case module_parser_state::INSTANCE: return "INSTANCE";
    case module_parser_state::ALWAYS_START: return "ALWAYS_START";
    case module_parser_state::ALWAYS_AT: return "ALWAYS_AT";
    case module_parser_state::ALWAYS_BODY: return "ALWAYS_BODY";
    case module_parser_state::ALWAYS_STATEMENT: return "ALWAYS_STATEMENT";
    case module_parser_state::ALWAYS_BLOCK: return "ALWAYS_BLOCK";
    case module_parser_state::ALWAYS_CASE: return "ALWAYS_CASE";
    case module_parser_state::PP_IFDEF: return "PP_IFDEF";
    case module_parser_state::DONE: return "DONE";
    }
    return "";
}

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

    option(const option<T>& assign)
    : _valid(assign._valid),
      _value(assign._value)
    {}

    const T& value(void) const {
        if (_valid == false) {
            std::cerr << "attempted to get value from empty option" << std::endl;
            abort();
        }

        return _value;
    }

    bool valid(void) const {
        return _valid;
    }
};

module::ptr parser::parse_module(const std::vector<lexer::token>& tokens, const std::unordered_map<std::string, pcad::module::ptr>& module_lookup)
{
    auto state = module_parser_state::TOP;

    auto name = option<std::string>(); 
    auto ports = std::vector<port::ptr>();

    auto port_direction = option<enum port_direction>();
    auto port_width = option<long>();

    auto scope_stack = std::stack<scope::ptr>();

    auto wire_name = option<std::string>();
    auto wire_width = option<long>();
    auto wire_depth = option<long>();

    auto instance_name = option<std::string>();

    auto body_scope = option<scope::ptr>();
    auto body_statements = std::vector<statement::ptr>();
    auto body_instances = std::vector<instance::ptr>();

    auto statement_tokens = std::vector<lexer::token>();

    auto instance_tokens = std::vector<lexer::token>();

    auto initial_begins = 0;

    auto always_begins = 0;
    auto always_at_tokens = std::vector<lexer::token>();
    auto always_statement_tokens = std::vector<lexer::token>();

    auto ifdef_depth = 0;

    for (const auto& token: tokens) {
#ifdef PCAD__DEBUG_MODULE_PARSER
        std::cerr << "parse_module(): state == " << to_string(state) << "\t" << token.str << std::endl;
#endif

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
                state = module_parser_state::BODY;
                scope_stack.push(std::make_shared<scope>(ports));
                body_scope = std::make_shared<scope>(scope_stack.top());
                scope_stack.push(body_scope.value());
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

        case module_parser_state::BODY:
            if (token == "assign") {
                statement_tokens.push_back(token);
                state = module_parser_state::ASSIGN_NAME;
            } else if (token == "initial") {
                state = module_parser_state::INITIAL_START;
            } else if (token == "wire") {
                state = module_parser_state::WIRE_NAME_OR_WIDTH;
            } else if (token == "reg") {
                state = module_parser_state::WIRE_NAME_OR_WIDTH;
            } else if (token == "integer") {
                state = module_parser_state::WIRE_NAME_OR_WIDTH;
            } else if (token == "always") {
                state = module_parser_state::ALWAYS_START;
            } else if (token == "`ifdef") {
                state = module_parser_state::PP_IFDEF;
                ifdef_depth = 1;
            } else if (token == "`ifndef") {
                state = module_parser_state::PP_IFDEF;
                ifdef_depth = 1;
            } else if (token == "`endif") {
                ifdef_depth--;
            } else {
                instance_tokens.push_back(token);
                state = module_parser_state::INSTANCE;
            }
            break;

        case module_parser_state::ASSIGN_NAME:
            statement_tokens.push_back(token);
            state = module_parser_state::ASSIGN_OP;
            break;
            
        case module_parser_state::ASSIGN_OP:
            if (token == "=") {
                statement_tokens.push_back(token);
                state = module_parser_state::ASSIGN_STATEMENT;
            } else {
                std::cerr << "Unknown assignment type " << token.str << "\n";
                abort();
            }
            break;

        case module_parser_state::ASSIGN_STATEMENT:
            if (token == ";") {
                state = module_parser_state::BODY;
                body_statements.push_back(
                    parse_statement(statement_tokens, scope_stack.top())
                );
                statement_tokens = {};
            } else {
                statement_tokens.push_back(token);
            }
            break;

        case module_parser_state::INITIAL_START:
            if (token == "begin") {
                initial_begins = 1;
                state = module_parser_state::INITIAL_BLOCK;
            } else
                state = module_parser_state::INITIAL_STATEMENT;
            break;

        case module_parser_state::INITIAL_STATEMENT:
            if (token == ";")
                state = module_parser_state::BODY;
            break;

        case module_parser_state::INITIAL_BLOCK:
            if (token == "begin")
                initial_begins++;
            if (token == "end")
                initial_begins--;

            if (initial_begins == 0)
                state = module_parser_state::BODY;
            break;

        case module_parser_state::WIRE_NAME_OR_WIDTH:
            if (token == "[") {
                if (wire_name.valid() == false)
                    state = module_parser_state::WIRE_WIDTH;
                else {
                    wire_width = 1;
                    state = module_parser_state::WIRE_DEPTH;
                }
            } else if (token == ";") {
                scope_stack.top()->add_var(
                    std::make_shared<wire>(
                        wire_name.value(),
                        1
                    ));
                wire_name = option<std::string>();
                wire_width = option<long>();
                wire_depth = option<long>();
                state = module_parser_state::BODY;
            } else {
                wire_name = token.str;
            }
            break;

        case module_parser_state::WIRE_WIDTH:
        {
            char *end = NULL;
            auto val = strtol(token.str.c_str(), &end, 0);
            if (end != NULL && *end == '\0') {
                wire_width = val + 1;
                state = module_parser_state::WIRE_NAME;
            } else {
                std::cerr << "unknown token " << token.str << " in module_parser_state::WIRE_WIDTH\n";
                abort();
            }

            break;
        }

        case module_parser_state::WIRE_NAME:
            if (token == "[") {
                state = module_parser_state::WIRE_DEPTH;
            } else if (token == ":") {
            } else if (token == "]") {
            } else if (token == "0") {
            } else if (token == ";") {
                scope_stack.top()->add_var(
                    std::make_shared<wire>(
                        wire_name.value(),
                        wire_width.value()
                    ));
                wire_name = option<std::string>();
                wire_width = option<long>();
                wire_depth = option<long>();
                state = module_parser_state::BODY;
            } else {
                wire_name = token.str;
            }
            break;

        case module_parser_state::WIRE_DEPTH:
        {
            char *end = NULL;
            auto val = strtol(token.str.c_str(), &end, 0);
            if (token == "[") {
            } else if (token == "0") {
                if (wire_depth.valid() == false)
                    wire_depth = 1;
            } else if (token == ":") {
            } else if (token == "]") {
            } else if (token == ";") {
                scope_stack.top()->add_var(
                    std::make_shared<wire>(
                        wire_name.value(),
                        wire_width.value(),
                        wire_depth.value()
                    ));
                wire_name = option<std::string>();
                wire_width = option<long>();
                wire_depth = option<long>();
                state = module_parser_state::BODY;
            } else if (end != NULL && *end == '\0') {
                wire_depth = val + 1;
                state = module_parser_state::WIRE_NAME;
            } else {
                std::cerr << "unknown token " << token.str << " in module_parser_state::WIRE_WIDTH\n";
                abort();
            }

            break;
        }

        case module_parser_state::INSTANCE:
            if (token == ";") {
                state = module_parser_state::BODY;
                auto instance = parse_instance(
                    instance_tokens,
                    module_lookup,
                    scope_stack.top());
                body_instances.push_back(instance);
                for (const auto& port: instance->ports())
                    scope_stack.top()->add_var(port);
                for (const auto& statement: instance->assignments())
                    body_statements.push_back(statement);

                instance_tokens = {};
            } else {
                instance_tokens.push_back(token);
            }
            break;

        case module_parser_state::ALWAYS_START:
            if (token == "@") {
                state = module_parser_state::ALWAYS_AT;
            } else {
                std::cerr << "Expected 'always @'\n";
                abort();
            }
            break;

        case module_parser_state::ALWAYS_AT:
            always_at_tokens.push_back(token);
            if (token == ")") {
                state = module_parser_state::ALWAYS_BODY;
            };
            break;

        case module_parser_state::ALWAYS_BODY:
            if (token == "begin") {
                state = module_parser_state::ALWAYS_BLOCK;
                always_statement_tokens = {token};
                always_begins = 1;
            } else if (token == "case") {
                state = module_parser_state::ALWAYS_CASE;
                always_statement_tokens = {token};
            } else {
                state = module_parser_state::ALWAYS_STATEMENT;
                always_statement_tokens = {token};
            }
            break;

        case module_parser_state::ALWAYS_STATEMENT:
            if (token == ";") {
                body_statements.push_back(
                    std::make_shared<always_statement>(
                        parse_statement(always_at_tokens, scope_stack.top()),
                        parse_statements(always_statement_tokens, scope_stack.top())
                    )
                );
                always_at_tokens = {};
                always_statement_tokens = {};
                state = module_parser_state::BODY;
            } else {
                always_statement_tokens.push_back(token);
            }
            break;

        case module_parser_state::ALWAYS_BLOCK:
            always_statement_tokens.push_back(token);
            if (token == "begin") {
                always_begins++;
            } else if (token == "end") {
                always_begins--;
            }

            if (token == "end" && always_begins == 0) {
                body_statements.push_back(
                    std::make_shared<always_statement>(
                        parse_statement(always_at_tokens, scope_stack.top()),
                        parse_statements(always_statement_tokens, scope_stack.top())
                    )
                );
                always_at_tokens = {};
                always_statement_tokens = {};
                state = module_parser_state::BODY;
            }
            break;

        case module_parser_state::ALWAYS_CASE:
            always_statement_tokens.push_back(token);
            if (token == "endcase") {
                body_statements.push_back(
                    std::make_shared<always_statement>(
                        parse_statement(always_at_tokens, scope_stack.top()),
                        parse_statements(always_statement_tokens, scope_stack.top())
                    )
                );
                always_at_tokens = {};
                always_statement_tokens = {};
                state = module_parser_state::BODY;
            }
            break;

        case module_parser_state::PP_IFDEF:
            if (token == "`ifdef") {
                ifdef_depth++;
            } else if (token == "`ifndef") {
                ifdef_depth++;
            } else if (token == "`endif") {
                ifdef_depth--;
            }

            if (ifdef_depth == 0)
                state = module_parser_state::BODY;
            break;

        case module_parser_state::DONE:
            break;
        }
    }

    return std::make_shared<module>(
        name.value(),
        ports,
        body_scope.value(),
        body_statements,
        body_instances
    );
}

enum parse_statements_state {
    BODY,
    BLOCK,
    IF_COND,
    IF_TRUE,
    IF_TRUE_STATEMENT,
    IF_TRUE_BLOCK,
    IF_FALSE,
    IF_FALSE_ELSE,
    IF_FALSE_ELSE_IF,
    IF_FALSE_ELSE_IF_COND,
    IF_FALSE_STATEMENT,
    IF_FALSE_BLOCK,
    FOR_START,
    FOR_HEADER,
    FOR_BODY,
    FOR_STATEMENT,
    FOR_BLOCK,
    ASSIGN_TARGET,
    ASSIGN_SOURCE,
};

std::string to_string(const enum parse_statements_state& state)
{
    switch (state) {
    case parse_statements_state::BODY: return "BODY";
    case parse_statements_state::BLOCK: return "BLOCK";
    case parse_statements_state::IF_COND: return "IF_COND";
    case parse_statements_state::IF_TRUE: return "IF_TRUE";
    case parse_statements_state::IF_TRUE_STATEMENT: return "IF_TRUE_STATEMENT";
    case parse_statements_state::IF_TRUE_BLOCK: return "IF_TRUE_BLOCK";
    case parse_statements_state::IF_FALSE: return "IF_FALSE";
    case parse_statements_state::IF_FALSE_ELSE: return "IF_FALSE_ELSE";
    case parse_statements_state::IF_FALSE_ELSE_IF: return "IF_FALSE_ELSE_IF";
    case parse_statements_state::IF_FALSE_ELSE_IF_COND: return "IF_FALSE_ELSE_IF_COND";
    case parse_statements_state::IF_FALSE_STATEMENT: return "IF_FALSE_STATEMENT";
    case parse_statements_state::IF_FALSE_BLOCK: return "IF_FALSE_BLOCK";
    case parse_statements_state::ASSIGN_TARGET: return "ASSIGN_TARGET";
    case parse_statements_state::ASSIGN_SOURCE: return "ASSIGN_SOURCE";
    case parse_statements_state::FOR_START: return "FOR_START";
    case parse_statements_state::FOR_HEADER: return "FOR_HEADER";
    case parse_statements_state::FOR_BODY: return "FOR_BODY";
    case parse_statements_state::FOR_STATEMENT: return "FOR_STATEMENT";
    case parse_statements_state::FOR_BLOCK: return "FOR_BLOCK";
    }

    abort();
}

std::vector<pcad::statement::ptr>
parser::parse_statements(const std::vector<lexer::token>& tokens,
                         const pcad::scope::ptr& scope)
{
    std::vector<pcad::statement::ptr> statements;
    auto state = parse_statements_state::BODY;

    auto begin_depth_mod = [](lexer::token token) {
        if (token == "begin") return 1;
        if (token == "end")   return -1;
        return 0;
    };

    auto parens_depth_mod = [](lexer::token token) {
        if (token == "(") return  1;
        if (token == ")") return -1;
        return 0;
    };

    auto begin_tokens = std::vector<lexer::token>();
    auto begin_depth = 0;

    auto if_depth = 0;
    auto if_cond_tokens = std::vector<lexer::token>();
    auto if_body_tokens = std::vector<lexer::token>();
    auto if_else_tokens = std::vector<lexer::token>();

    auto assign_target = std::vector<lexer::token>();
    auto assign_source = std::vector<lexer::token>();

    auto for_depth = 0;
    auto for_header = std::vector<lexer::token>();
    auto for_body = std::vector<lexer::token>();

#ifdef PCAD__DEBUG_STATEMENTS_PARSER
    std::cerr << "parse_stements()\n";
#endif

    for (const auto& token: tokens) {
#ifdef PCAD__DEBUG_STATEMENTS_PARSER
        std::cerr << "parse_statements(state = " << to_string(state) << "): " << token.str << "\n";
#endif

        switch (state) {
        case parse_statements_state::IF_FALSE:
            if (token == "else") {
                state = parse_statements_state::IF_FALSE_ELSE;
                break;
            } else {
                statements.push_back(
                    std::make_shared<if_statement>(
                        parse_statement(if_cond_tokens, scope),
                        parse_statements(if_body_tokens, scope),
                        parse_statements(if_else_tokens, scope)
                    )
                );

                state = parse_statements_state::BODY;
                if_depth = 0;
                if_cond_tokens = {};
                if_body_tokens = {};
                if_else_tokens = {};
            }
            /* That's right: there's a conditional fall-through here! */

        case parse_statements_state::BODY:
            if (token == "begin") {
                begin_depth = begin_depth_mod(token);
                state = parse_statements_state::BLOCK;
            } else if (token == "if") {
                if_depth = 0;
                if_cond_tokens = {};
                if_body_tokens = {};
                if_else_tokens = {};

                state = parse_statements_state::IF_COND;
            } else if (token == "for") {
                for_depth = 0;
                for_header = {};
                for_body = {};
                state = parse_statements_state::FOR_HEADER;
            } else {
                assign_target = {token};
                state = parse_statements_state::ASSIGN_TARGET;
            }
            break;

        case parse_statements_state::BLOCK:
            begin_depth += begin_depth_mod(token);
            if (begin_depth == 0) {
                for (const auto& statement: parse_statements(begin_tokens, scope))
                    statements.push_back(statement);
                begin_tokens = {};
                begin_depth = 0;
                state = parse_statements_state::BODY;
            } else {
                begin_tokens.push_back(token);
            }
            break;

        case parse_statements_state::IF_COND:
            if_depth += parens_depth_mod(token);
            if_cond_tokens.push_back(token);
            if (if_depth == 0) {
                state = parse_statements_state::IF_TRUE;
            }
            break;

        case parse_statements_state::IF_TRUE:
            if (token == "begin") {
                state = parse_statements_state::IF_TRUE_BLOCK;
                if_depth = 1;
                if_body_tokens = {token};
            } else {
                if_body_tokens.push_back(token);
                state = parse_statements_state::IF_TRUE_STATEMENT;
            }
            break;

        case parse_statements_state::IF_TRUE_STATEMENT:
            if (token == ";") {
                state = parse_statements_state::IF_FALSE;
            } else {
                if_body_tokens.push_back(token);
            }
            break;

        case parse_statements_state::IF_TRUE_BLOCK:
            if_body_tokens.push_back(token);
            if_depth += begin_depth_mod(token);
            if (if_depth == 0)
                state = parse_statements_state::IF_FALSE;
            break;

        case parse_statements_state::IF_FALSE_ELSE:
            if (token == "begin") {
                state = parse_statements_state::IF_FALSE_BLOCK;
                if_depth = 1;
            } else if (token == "if") {
                state = parse_statements_state::IF_FALSE_ELSE_IF;
                if_else_tokens = {token};
            } else {
                if_else_tokens = {token};
                state = parse_statements_state::IF_FALSE_STATEMENT;
            }
            break;

        case parse_statements_state::IF_FALSE_STATEMENT:
            if (token == ";") {
                state = parse_statements_state::IF_FALSE;
            } else {
                if_else_tokens.push_back(token);
            }
            break;

        case parse_statements_state::IF_FALSE_BLOCK:
            if_else_tokens.push_back(token);
            if_depth += begin_depth_mod(token);
            if (if_depth == 0)
                state = parse_statements_state::IF_FALSE;
            break;

        case parse_statements_state::IF_FALSE_ELSE_IF:
            if (token == "(") {
                if_depth = 1;
                if_else_tokens.push_back(token);
                state = parse_statements_state::IF_FALSE_ELSE_IF_COND;
            } else {
                std::cerr << "expected ( at start of token\n";
                abort();
            }
            break;

        case parse_statements_state::IF_FALSE_ELSE_IF_COND:
            if_depth += parens_depth_mod(token);
            if_else_tokens.push_back(token);
            if (if_depth == 0) {
                state = parse_statements_state::IF_FALSE_ELSE;
            }
            break;

        case parse_statements_state::ASSIGN_TARGET:
            if (token == "<=") {
                state = parse_statements_state::ASSIGN_SOURCE;
            } else {
                assign_target.push_back(token);
            }
            break;

        case parse_statements_state::ASSIGN_SOURCE:
            if (token == ";") {
                statements.push_back(
                    std::make_shared<assign_statement>(
                        parse_statement(assign_target, scope),
                        parse_statement(assign_source, scope)
                    )
                );
                assign_target = {};
                assign_source = {};
                state = parse_statements_state::BODY;
            } else {
                assign_source.push_back(token);
            }
            break;

        case parse_statements_state::FOR_START:
            if (token == "(") {
                for_header = {token};
                state = parse_statements_state::FOR_HEADER;
                for_depth = 1;
            } else {
                std::cerr << "Unknown token in FOR_START: " << token.str << "\n";
                abort();
            }
            break;

        case parse_statements_state::FOR_HEADER:
            for_header.push_back(token);
            for_depth += parens_depth_mod(token);
            if (for_depth == 0)
                state = parse_statements_state::FOR_BODY;
            break;

        case parse_statements_state::FOR_BODY:
            if (token == "begin") {
                for_depth = 1;
                for_body = {token};
                state = parse_statements_state::FOR_BLOCK;
            } else {
                for_body = {token};
                state = parse_statements_state::FOR_STATEMENT;
            }
            break;

        case parse_statements_state::FOR_STATEMENT:
        case parse_statements_state::FOR_BLOCK:
            break;
        }
    }

    return statements;
}

pcad::statement::ptr parser::parse_statement(const std::vector<lexer::token>& tokens,
                                             const pcad::scope::ptr& scope)
{
#ifdef PCAD__DEBUG_STATEMENT_PARSER
    std::cerr << "statement: ";
    for (const auto& token: tokens)
        std::cerr << " '" << token.str << "'";
    std::cerr << std::endl;
#endif

    /* Some helper functions. */
    auto depth_mod = [](lexer::token token) {
        if (token == "(") return  1;
        if (token == ")") return -1;
        if (token == "{") return  1;
        if (token == "}") return -1;
        if (token == "[") return  1;
        if (token == "]") return -1;
        return 0;
    };

    /* Here's the special cases that don't look like a recursive op parser. */
    if (tokens.size() == 0) {
        std::cerr << "asked to parse empty token set\n";
        abort();
        return nullptr;
    }

    if (tokens.size() == 1)
        return std::make_shared<wire_statement>(parse_wire(tokens[0], scope));

    if (tokens[0] == "assign") {
        auto rest = std::vector<lexer::token>();
        for (auto it = begin(tokens) + 3; it != end(tokens); ++it)
            rest.push_back(*it);

        return std::make_shared<assign_statement>(
            parse_statement({tokens[1]}, scope),
            parse_statement(rest, scope)
        );
    }

    if (tokens.size() == 2 && tokens[0] == "posedge") {
        return std::make_shared<posedge_statement>(
            parse_wire(tokens[1], scope)
        );
    }

    /* The highest level Verilog operator is the ternery op. */
    if (tokens.size() >= 4) {
        auto sel = std::vector<lexer::token>();
        auto ont = std::vector<lexer::token>();
        auto onf = std::vector<lexer::token>();
        size_t i = 0;
        size_t depth = 0;

        while (i < tokens.size() && (depth >= 1 || tokens[i] != "?")) {
            sel.push_back(tokens[i]);
            depth += depth_mod(tokens[i]);
            i++;
        }
        i++;

        while (i < tokens.size() && (depth >= 1 || tokens[i] != ":")) {
            ont.push_back(tokens[i]);
            depth += depth_mod(tokens[i]);
            i++;
        }
        i++;

        while (i < tokens.size()) {
            onf.push_back(tokens[i]);
            depth += depth_mod(tokens[i]);
            i++;
        }

        if (sel.size() > 0 && ont.size() > 0 && onf.size() > 0) {
            return std::make_shared<trop_statement>(
                parse_statement(sel, scope),
                parse_statement(ont, scope),
                parse_statement(onf, scope)
            );
        }
    }

    /* The bit-slice syntax is odd and I'm not sure how to fit it into a
     * regular in-fix parser, so I'm kind of just hacking it together here. */
    if (tokens.size() == 4 && tokens[1] == "[" && tokens[3] == "]") {
        return std::make_shared<slice_statement>(
            parse_statement({tokens[0]}, scope),
            parse_statement({tokens[2]}, scope),
            parse_statement({tokens[2]}, scope)
        );
    }

    if (tokens.size() == 6 && tokens[1] == "[" && tokens[3] == ":" && tokens[5] == "]") {
        return std::make_shared<slice_statement>(
            parse_statement({tokens[0]}, scope),
            parse_statement({tokens[2]}, scope),
            parse_statement({tokens[4]}, scope)
        );
    }

    /* Man, parsing Verilog is wacky -- in this case the "{" token can actually
     * mean a whole host of operators... */
    if (tokens.size() >= 3 && tokens[0] == "{") {
        /* This is a repeat operator, which is kind of this crazy binary op
         * that doesn't fit the regular pattern. */
        if (parse_literal(tokens[1]) != nullptr && tokens[2] == "{") {
            auto rest = std::vector<lexer::token>();
            for (size_t i = 3; i < tokens.size() - 2; ++i)
                rest.push_back(tokens[i]);
            return std::make_shared<rep_statement>(
                parse_literal(tokens[1]),
                parse_statement(rest, scope)
            );
        }

        /* We can also "concatonate" one thing, which is just a NOP. */
        if (tokens.size() == 3 && tokens[0] == "{" && tokens[2] == "}")
            return parse_statement({tokens[1]}, scope);

        /* If it's not a repetition operation then hopefully it's a
         * concatonation operator.  Let's try and see... */
        if (tokens[tokens.size()-1] == "}") {
            auto hi = std::vector<lexer::token>();
            auto lo = std::vector<lexer::token>();
            size_t i = 1;
            size_t depth = 1;

            while (i < tokens.size() && (depth > 1 || tokens[i] != ",")) {
                if (tokens[i] == "{") depth++;
                if (tokens[i] == "}") depth--;
                hi.push_back(tokens[i]);
                i++;
            }
            i++;

            while (i < (tokens.size() - 1)) {
                if (tokens[i] == "{") depth++;
                if (tokens[i] == "}") depth--;
                lo.push_back(tokens[i]);
                i++;
            }

            return std::make_shared<cat_statement>(
                parse_statement(hi, scope),
                parse_statement(lo, scope)
            );
        }

        std::cerr << "I can't figure out your {\n";
        abort();
    }

    /* Here's the generic parsing stuff: this tries to split the statement into
     * two parts: one on either half of a binary operation. */
    if (tokens.size() >= 3) {
        size_t depth = 0;
        auto biop_offset = option<size_t>();
        auto biop_op = option<biop_statement::op>();
        auto lhs = std::vector<lexer::token>();
        auto rhs = std::vector<lexer::token>();
        for (size_t i = 0; i < tokens.size(); ++i) {
            auto token = tokens[i];

            /* Checks to see if this is a binary operation and if so splits the
             * parse right there. */
            auto biop = [&]() -> option<biop_statement::op> {
                if (token == "|")   return biop_statement::op::OR;
                if (token == "==")  return biop_statement::op::EQEQ;
                if (token == "+")   return biop_statement::op::ADD;
                if (token == "&")   return biop_statement::op::AND;
                if (token == "<")   return biop_statement::op::LT;
                if (token == "!=")  return biop_statement::op::NEQ;
                if (token == ">=")  return biop_statement::op::GTEQ;
                if (token == "<<")  return biop_statement::op::LSH;
                if (token == ">>")  return biop_statement::op::RSHL;
                if (token == "<=")  return biop_statement::op::LTEQ;
                if (token == "^")   return biop_statement::op::XOR;
                if (token == ">>>") return biop_statement::op::RSHA;
                if (token == "-")   return biop_statement::op::SUB;
                if (token == "*")   return biop_statement::op::MUL;
                if (token == ">")   return biop_statement::op::GT;
                return option<biop_statement::op>();
            }();

            if (biop_op.valid() == false && (depth > 0 || biop.valid() == false))
                lhs.push_back(token);
            if (biop_op.valid() == true)
                rhs.push_back(token);

            /* Checks to make sure we're outside of parens and such before
             * splitting the op up. */
            depth += depth_mod(token);

            if (depth == 0 && biop_op.valid() == false && biop.valid() == true) {
                /* FIXME: what about the direction of associativity and
                 * operator precedence? */
                biop_offset = i;
                biop_op = biop;
            }
        }

        if (biop_offset.valid()) {
            return std::make_shared<biop_statement>(
                biop_op.value(),
                parse_statement(lhs, scope),
                parse_statement(rhs, scope)
            );
        }
    }

    /* Unary ops have a lower precedence than binary ops. */
    if (tokens.size() >= 2) {
        auto unop_op = [&]() -> option<unop_statement::op> {
            if (tokens[0] == "~")  return unop_statement::op::NOT;
            return option<unop_statement::op>();
        }();

        if (unop_op.valid()) {
            auto rest = std::vector<lexer::token>();
            for (auto it = begin(tokens) + 1; it != end(tokens); ++it)
                rest.push_back(*it);
            return std::make_shared<unop_statement>(
                unop_op.value(),
                parse_statement(rest, scope)
            );
        }
    }

    /* The signed stuff is the lowest priority */
    if (tokens[0] == "$signed" || tokens[0] == "$unsigned") {
        auto rest = std::vector<lexer::token>();
        for (auto it = begin(tokens) + 1; it != end(tokens); ++it)
            rest.push_back(*it);

        return std::make_shared<builtin_statement>(
            tokens[0].str,
            parse_statement(rest, scope)
        );
    }

    /* Sometimes I end up with just parens, so I strip them.  This happens last
     * because if I can find any other operation then I actually want to use
     * that. */
    if (tokens[0] == "(") {
        auto rest = std::vector<lexer::token>();
        for (size_t i = 1; i < tokens.size() - 1; i++)
            rest.push_back(tokens[i]);
        return parse_statement(rest, scope);
    }

    std::cerr << "Unable to parse statement:\n";
    for (const auto& token: tokens)
        std::cerr << "token: " << token.str << "\n";
    abort();
    return nullptr;
}

pcad::wire::ptr parser::parse_wire(const lexer::token& token,
                                   const pcad::scope::ptr& scope)
{
    if (token == "$random")
        return std::make_shared<pcad::random>();

    if (token == "*")
        return std::make_shared<pcad::star>();

    auto lit = parse_literal(token);
    if (lit != nullptr)
        return lit;

    auto wire = scope->lookup(token.str);
    if (wire == nullptr) {
        std::cerr << "Unable to find wire " << token.str << "\n";
        abort();
    }
    return wire;
}

pcad::literal::ptr parser::parse_literal(const lexer::token& token)
{
    auto apos = token.str.find('\'');

    char *end = NULL;
    auto width = strtol(token.str.c_str(), &end, 10);
    if (apos == std::string::npos) {
        apos = 0;
        width = 32;
    } else {
        apos++;
    }

    if (token.str[apos] == 's')
        apos++;

    auto bitwidth = 10;
    if (token.str[apos] == 'h') {
        bitwidth = 16;
        apos++;
    } else if (token.str[apos] == 'd') {
        bitwidth = 10;
        apos++;
    } else if (token.str[apos] == 'b') {
        bitwidth = 2;
        apos++;
    }


    auto val = strtol(token.str.c_str() + apos, &end, bitwidth);
    if (end != NULL && *end == '\0') {
        return std::make_shared<pcad::literal>(val, width);
    } else {
        return nullptr;
    }
}

enum instance_parser_state {
    MODULE_NAME,
    INSTANCE_NAME,
    PORTS,
    PORT_DOT,
    PORT_NAME,
    PORT_STATEMENT,
    PORT_COMMA,
    PORT_END,
};

pcad::instance::ptr parser::parse_instance(const std::vector<lexer::token>& tokens,
                                           const std::unordered_map<std::string, pcad::module::ptr>& module_lookup,
                                           const pcad::scope::ptr& scope)
{
    auto parens_depth_mod = [](lexer::token token) {
        if (token == "(") return  1;
        if (token == ")") return -1;
        return 0;
    };

    auto state = instance_parser_state::MODULE_NAME;

    auto instance_name = option<std::string>();

    auto port_name = option<std::string>();
    auto port_tokens = std::vector<lexer::token>();

    auto ports = std::vector<port::ptr>();
    auto name2port = std::unordered_map<std::string, port::ptr>();

    auto module = option<pcad::module::ptr>();

    auto assignments = std::vector<statement::ptr>();
    auto assign_port_tokens = std::vector<lexer::token>();
    auto assign_statement_tokens = std::vector<lexer::token>();
    auto assign_depth = 0;

    scope::ptr port_scope = nullptr;

    for (const auto& token: tokens) {
        switch (state) {
        case instance_parser_state::MODULE_NAME:
        {
            auto l = module_lookup.find(token.str);
            if (l == module_lookup.end()) {
                std::cerr << "unable to find module " << token.str << "\n";
                abort();
            }
            module = l->second;
            for (const auto& port_in_module: module.value()->ports()) {
                auto port_in_instance = std::make_shared<port>(
                    port_in_module->name(),
                    port_in_module->width(),
                    port_in_module->direction()
                );
                ports.push_back(port_in_instance);
                name2port[port_in_instance->name()] = port_in_instance;
            }
            port_scope = std::make_shared<pcad::scope>(ports);
            state = instance_parser_state::INSTANCE_NAME;
            break;
        }

        case instance_parser_state::INSTANCE_NAME:
            instance_name = token.str;

            for (const auto& port: module.value()->ports()) {
                ports.push_back(
                    std::make_shared<pcad::port>(
                        token.str + "." + port->name(),
                        port->width(),
                        port->direction()
                    )
                );
            }

            state = instance_parser_state::PORTS;
            break;

        case instance_parser_state::PORTS:
            if (token == "(") {
                state = instance_parser_state::PORT_DOT;
            } else {
                std::cerr << "Expected ( at start of module port list\n";
                abort();
            }
            break;

        case instance_parser_state::PORT_DOT:
            if (token.str[0] == '.') {
                assign_port_tokens = {
                    lexer::token(
                        token.str.substr(1),
                        token.line,
                        token.col+1
                    )
                };
                state = instance_parser_state::PORT_NAME;
            } else {
                std::cerr << "Expected . in module port list\n";
                abort();
            }
            break;

        case instance_parser_state::PORT_NAME:
            if (token == "(") {
                assign_statement_tokens = {token};
                assign_depth = 1;
                state = instance_parser_state::PORT_STATEMENT;
            } else {
                assign_port_tokens.push_back(token);
            }
            break;

        case instance_parser_state::PORT_STATEMENT:
            assign_statement_tokens.push_back(token);
            assign_depth += parens_depth_mod(token);
            if (assign_depth == 0) {
                assignments.push_back(
                    std::make_shared<assign_statement>(
                        parse_statement(assign_port_tokens, port_scope),
                        parse_statement(assign_statement_tokens, scope)
                    )
                );

                state = instance_parser_state::PORT_COMMA;
                assign_statement_tokens = {};
                assign_port_tokens = {};
                assign_depth = 0;
            }
            break;

        case instance_parser_state::PORT_COMMA:
            if (token == ",") {
                state = instance_parser_state::PORT_DOT;
            } else if (token == ")") {
                state = instance_parser_state::PORT_END;
            }
            break;

        case instance_parser_state::PORT_END:
            break;
        }
    }

    return std::make_shared<instance>(
        instance_name.value(),
        module.value(),
        ports,
        assignments
    );
}
