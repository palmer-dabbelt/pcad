// See LICENSE for details

#ifndef PCAD__HDLAST__INSTANCE_HXX
#define PCAD__HDLAST__INSTANCE_HXX

namespace pcad {
    namespace hdlast {
        class instance;
    }
}

#include "module.h++"
#include "statement.h++"
#include <memory>

namespace pcad {
    namespace hdlast {
        /* A single instance of a module.  Since this has to support Verilog,
         * assignment to instances actually arrive via the */
        class instance {
        public:
            typedef std::shared_ptr<instance> ptr;

        private:
            const std::string _name;
            const std::shared_ptr<hdlast::module> _module;
            const std::vector<port::ptr> _ports;
            const std::vector<std::shared_ptr<assign_statement>> _assignments;

        public:
            instance(const std::string& name,
                     const decltype(_module)& module,
                     const std::vector<port::ptr>& ports,
                     const decltype(_assignments)& assignments)
            : _name(name),
              _module(module),
              _ports(ports),
              _assignments(assignments)
            {
            }

        public:
            const decltype(_name)& name(void) const { return _name; }
            const decltype(_module)& module(void) const { return _module; }
            const decltype(_ports)& ports(void) const { return _ports; }
            const decltype(_assignments)& assignments(void) const
            { return _assignments; }
        };
    }
}

#endif
