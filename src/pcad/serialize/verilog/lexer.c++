// See LICENSE for details

#include "lexer.h++"
#include <cstring>
#include <fstream>
#include <iostream>
using namespace pcad::serialize::verilog;

enum class lexer_state {
    BODY,
    LINE_COMMENT,
    STRING,
    BANG,
    SLASH,
};

static std::vector<lexer::token> lex(const std::vector<std::string>& filename);
static std::vector<lexer::token> lex(std::ifstream& i);
static bool isbreak(int c);

lexer::lexer(const std::vector<std::string>& filenames)
: _tokens(lex(filenames))
{
}

bool isbreak(int c)
{
    if (isspace(c)) return true;
    if (c == ',' || c == ';') return true;
    if (c == '(' || c == ')') return true;
    if (c == '[' || c == ':' || c == ']') return true;
    if (c == '+' || c == '-' || c == '*' || c == '^') return true;
    if (c == '~') return true;
    if (c == '{' || c == '}') return true;
    return false;
}

std::vector<lexer::token> lex(const std::vector<std::string>& filenames)
{
    std::vector<lexer::token> out;
    for (const auto& filename: filenames) {
        std::ifstream ifs(filename);
        for (const auto& token: lex(ifs))
            out.push_back(token);
    }
    return out;
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
            if (cur_c == '"') {
                token_string = token_string + cur_s;
                state = lexer_state::STRING;
            } else if (cur_c == '!') {
                state = lexer_state::BANG;
                if (token_string.size() > 0) {
                    tokens.push_back(lexer::token(token_string, line, col));
                    token_string = "";
                }
            } else if (cur_c == '/') {
                state = lexer_state::SLASH;
                if (token_string.size() > 0) {
                    tokens.push_back(lexer::token(token_string, line, col));
                    token_string = "";
                }
            } else if (isbreak(cur_c)) {
                if (token_string.size() > 0) {
                    tokens.push_back(lexer::token(token_string, line, col));
                    token_string = "";
                }

                if (!isspace(cur_c))
                    tokens.push_back(lexer::token(cur_s, line, col));
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

        /* ! is a special character -- sometimes it causes a lexing token
         * break, sometimes it doesn't. */
        case lexer_state::BANG:
            if (cur_c == '=') {
                tokens.push_back(lexer::token("!=", line, col));
                token_string = "";
            } else {
                tokens.push_back(lexer::token("!", line, col));
                token_string = cur_s;
            }
            state = lexer_state::BODY;
            break;

        /* Slashes are also a special case: they might trigger a line comment.
         * Handle this like !. */
        case lexer_state::SLASH:
            if (cur_c == '/') {
                state = lexer_state::LINE_COMMENT;
            } else {
                tokens.push_back(lexer::token("/", line, col));
                token_string = cur_s;
                state = lexer_state::BODY;
            }
            break;
        }

        prev_c = cur_c;
    }

    return tokens;
}
