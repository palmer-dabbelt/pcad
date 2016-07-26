// See LICENSE for details

#ifndef PCAD__MODULE_HXX
#define PCAD__MODULE_HXX

#include "decoupled.h++"
#include "port.h++"
#include "scope.h++"
#include "statement.h++"
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
        const std::vector<statement::ptr> _statements;

    public:
        module(const decltype(_name)& name,
               const decltype(_ports)& ports,
               const decltype(_body)& body,
               const decltype(_statements)& statements)
        : _name(name),
          _ports(ports),
          _body(body),
          _statements(statements)
        {}

    public:
        const decltype(_name)& name(void) const { return _name; }
        const decltype(_ports)& ports(void) const { return _ports; }
        const decltype(_body)& body(void) const { return _body; }
        const decltype(_statements)& statements(void) const { return _statements; }

    public:
        /* Infers decoupled IOs using port names.  This is fragile and will
         * probably only work for Rocket Chip-like things, but I don't care
         * that much. */
        std::vector<decoupled::ptr> infer_decoupled_io(void) const;
    };
}

#endif
