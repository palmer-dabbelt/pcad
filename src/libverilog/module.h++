// See LICENSE for details

#ifndef LIBVERILOG__MODULE_HXX
#define LIBVERILOG__MODULE_HXX

#include "lexer.h++"
#include "port.h++"
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
        const std::vector<port::ptr> _ports;

    public:
        module(const decltype(_name)& name,
               const decltype(_ports)& ports);

    public:
        const decltype(_name)& name(void) const { return _name; }
        const decltype(_ports)& ports(void) const { return _ports; }
    };
}

#endif
