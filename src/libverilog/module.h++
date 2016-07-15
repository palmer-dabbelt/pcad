// See LICENSE for details

#ifndef LIBVERILOG__MODULE_HXX
#define LIBVERILOG__MODULE_HXX

#include "lexer.h++"
#include <memory>
#include <string>
#include <vector>

namespace libverilog {
    /* A Verilog circuit is just a collection of modules. */
    class module {
    public:
        typedef std::shared_ptr<module> ptr;

    public:
        const std::string _name;

    public:
        module(const std::string& name);

    public:
        const decltype(_name)& name(void) const { return _name; }
    };
}

#endif
