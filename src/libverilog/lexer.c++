// See LICENSE for details

#include "lexer.h++"
#include <cstring>
#include <fstream>
#include <iostream>
using namespace libverilog;

enum class lexer_state {
    BODY,
    LINE_COMMENT,
    STRING,
};

static std::vector<lexer::token> lex(const std::string& filename);
static std::vector<lexer::token> lex(std::ifstream& i);
static bool isbreak(int c);

lexer::lexer(const std::string& filename)
: _tokens(lex(filename))
{
}

bool isbreak(int c)
{
    if (isspace(c)) return true;
    if (c == ',') return true;
    if (c == '(' || c == ')') return true;
    if (c == '+' || c == '-' || c == '*' || c == '^') return true;
    return false;
}

std::vector<lexer::token> lex(const std::string& filename)
{
    std::ifstream ifs(filename);
    return lex(ifs);
}

std::vector<lexer::token> lex(std::ifstream& i)
{
    std::vector<lexer::token> tokens;
    std::string token_string;

    size_t line = 1;
    size_t col = 0;
    auto state = lexer_state::BODY;

    auto prev_c = '\0';
    while (!i.eof()) {
        auto cur_c = i.get();
        if (i.eof())
            break;

        char prev_s[3];
        prev_s[0] = prev_c;
        prev_s[1] = cur_c;
        prev_s[2] = '\0';
        auto cur_s = &prev_s[1];

        /* Here we track the debug info. */
        col++;
        if (cur_c == '\n') {
            line++;
            col = 0;
        }

        switch (state) {
        case lexer_state::BODY:
            if (std::strcmp(prev_s, "//") == 0) {
                token_string = token_string.substr(0, token_string.size() - 1);
                if (token_string.size() > 0) {
                    tokens.push_back(lexer::token(token_string, line, col));
                    token_string = "";
                }
                state = lexer_state::LINE_COMMENT;
            } else if (cur_c == '"') {
                token_string = token_string + cur_s;
                state = lexer_state::STRING;
            } else if (isbreak(cur_c)) {
                if (token_string.size() > 0) {
                    tokens.push_back(lexer::token(token_string, line, col));
                    token_string = "";
                }
            } else {
                token_string = token_string + cur_s;
            }
            break;

        /* A comment that goes to the end of the line. */
        case lexer_state::LINE_COMMENT:
            if (cur_c == '\n')
                state = lexer_state::BODY;
            break;

        /* A string doesn't break when there are control characters. */
        case lexer_state::STRING:
            token_string = token_string + cur_s;
            if (cur_c == '"' && prev_c != '\\') {
                tokens.push_back(lexer::token(token_string, line, col));
                token_string = "";
                state = lexer_state::BODY;
            }
            break;
        }

        prev_c = cur_c;
    }

    return tokens;
}
