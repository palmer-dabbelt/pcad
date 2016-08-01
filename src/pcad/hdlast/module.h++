// See LICENSE for details

#ifndef PCAD__HDLAST__MODULE_HXX
#define PCAD__HDLAST__MODULE_HXX

namespace pcad {
    namespace hdlast {
        class module;
    }
}

#include "instance.h++"
#include "port.h++"
#include "scope.h++"
#include "statement.h++"
#include <memory>
#include <string>
#include <vector>

namespace pcad {
    namespace hdlast {
        /* A Verilog circuit is just a collection of modules. */
        class module {
        public:
            typedef std::shared_ptr<module> ptr;

        public:
            const std::string _name;
            const std::vector<port::ptr> _ports;
            const scope::ptr _body;
            const std::vector<statement::ptr> _statements;
            const std::vector<std::shared_ptr<instance>> _instances;

        public:
            module(const decltype(_name)& name,
                   const decltype(_ports)& ports,
                   const decltype(_body)& body,
                   const decltype(_statements)& statements,
                   const decltype(_instances)& instance)
            : _name(name),
              _ports(ports),
              _body(body),
              _statements(statements),
              _instances(instance)
            {}

        public:
            const decltype(_name)& name(void) const { return _name; }
            const decltype(_ports)& ports(void) const { return _ports; }
            const decltype(_body)& body(void) const { return _body; }
            const decltype(_statements)& statements(void) const { return _statements; }
            const decltype(_instances)& instances(void) const { return _instances; }
        };
    }
}

#endif
