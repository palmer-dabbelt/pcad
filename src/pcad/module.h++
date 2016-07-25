// See LICENSE for details

#ifndef PCAD__MODULE_HXX
#define PCAD__MODULE_HXX

#include "decoupled.h++"
#include "port.h++"
#include "scope.h++"
#include <memory>
#include <string>
#include <vector>

namespace pcad {
    /* A Verilog circuit is just a collection of modules. */
    class module {
    public:
        typedef std::shared_ptr<module> ptr;

    public:
        const std::string _name;
        const std::vector<port::ptr> _ports;
        const scope::ptr _body;

    public:
        module(const decltype(_name)& name,
               const decltype(_ports)& ports,
               const decltype(_body)& body);

    public:
        const decltype(_name)& name(void) const { return _name; }
        const decltype(_ports)& ports(void) const { return _ports; }
        const decltype(_body)& body(void) const { return _body; }

    public:
        /* Infers decoupled IOs using port names.  This is fragile and will
         * probably only work for Rocket Chip-like things, but I don't care
         * that much. */
        std::vector<decoupled::ptr> infer_decoupled_io(void) const;
    };
}

#endif
