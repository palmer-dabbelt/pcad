// See LICENSE for details

#ifndef LIBVERILOG__LEXER_HXX
#define LIBVERILOG__LEXER_HXX

#include <memory>
#include <vector>
#include <string>

namespace libverilog {
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
        lexer(const std::string& filename);

    public:
        const decltype(_tokens)& tokens(void) const { return _tokens; }
    };
}

static inline
bool operator==(const libverilog::lexer::token& t, const std::string& s)
{
    return t.str == s;
}

static inline
bool operator!=(const libverilog::lexer::token& t, const std::string& s)
{
    return !(t == s);
}

#endif
