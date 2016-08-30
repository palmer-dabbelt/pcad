// See LICENSE for details

#ifndef PCAD__SERIALIZE__VERILOG__LEXER_HXX
#define PCAD__SERIALIZE__VERILOG__LEXER_HXX

#include <memory>
#include <vector>
#include <string>

namespace pcad {
    namespace serialize {
        namespace verilog {
            /* A Verilog lexer.  You probably don't want to use this directly, use the
             * parser instead. */
            class lexer {
            public:
                typedef std::shared_ptr<lexer> ptr;

                /* The lexer preserves debug info. */
                class token {
                public:
                    std::string str;
                    size_t line;
                    size_t col;

                    token(const std::string& _str, size_t _line, size_t _col)
                    : str(_str),
                      line(_line),
                      col(_col)
                    { }
                };

            private:
                std::vector<token> _tokens;

            public:
                lexer(const std::vector<std::string>& filenames);

            public:
                const decltype(_tokens)& tokens(void) const { return _tokens; }
            };
        }
    }
}

static inline
bool operator==(const pcad::serialize::verilog::lexer::token& t, const std::string& s)
{
    return t.str == s;
}

static inline
bool operator!=(const pcad::serialize::verilog::lexer::token& t, const std::string& s)
{
    return !(t == s);
}

static inline
pcad::serialize::verilog::lexer::token
operator+(const pcad::serialize::verilog::lexer::token& a,
          const pcad::serialize::verilog::lexer::token& b)
{
    return pcad::serialize::verilog::lexer::token(a.str + b.str, a.line, a.col);
}

#endif
