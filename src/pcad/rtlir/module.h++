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

        public:
            virtual const decltype(_name)& name(void) const { return _name; }
            virtual const decltype(_ports)& ports(void) const { return _ports; }
            virtual const decltype(_instances)& instances(void) const { return _instances; }
        };
    }
}

#endif
