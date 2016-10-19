// See LICENSE for details

#ifndef PCAD__RTLIR__MODULE_HXX
#define PCAD__RTLIR__MODULE_HXX

namespace pcad {
    namespace rtlir {
        class module;
    }
}

#include "instance.h++"
#include "port.h++"
#include <memory>
#include <string>
#include <vector>

namespace pcad {
    namespace rtlir {
        /* This does moduley things, but in RTL. */
        class module {
        public:
            typedef std::shared_ptr<module> ptr;

        private:
            const std::string _name;
            const std::vector<std::shared_ptr<instance>> _instances;
            const std::vector<std::shared_ptr<port>> _ports;
            const std::vector<std::shared_ptr<wire>> _wires;
            const std::vector<std::shared_ptr<statement>> _statements;

        public:
            module(const decltype(_name)& name)
            : _name(name)
            {}

            module(const decltype(_name)& name,
                   const decltype(_instances)& instances)
            : _name(name),
              _instances(instances)
            {}

            module(const decltype(_name)& name,
                   const decltype(_ports)& ports,
                   const decltype(_instances)& instances)
            : _name(name),
              _instances(instances),
              _ports(ports)
            {}

            module(const decltype(_name)& name,
                   const decltype(_wires)& wires,
                   const decltype(_ports)& ports,
                   const decltype(_instances)& instances)
            : _name(name),
              _instances(instances),
              _ports(ports),
              _wires(wires)
            {}

            module(const decltype(_name)& name,
                   const decltype(_wires)& wires,
                   const decltype(_ports)& ports,
                   const decltype(_statements)& statements,
                   const decltype(_instances)& instances)
            : _name(name),
              _instances(instances),
              _ports(ports),
              _wires(wires),
              _statements(statements)
            {}

        public:
            virtual const decltype(_name)& name(void) const { return _name; }
            virtual const decltype(_ports)& ports(void) const { return _ports; }
            virtual const decltype(_instances)& instances(void) const { return _instances; }
            virtual const decltype(_wires)& wires(void) const { return _wires; }
            virtual const decltype(_statements)& statements(void) const { return _statements; }
        };
    }
}

#endif
