// See LICENSE for details

#ifndef PCAD__RTLIR__INTSANCE_HXX
#define PCAD__RTLIR__INTSANCE_HXX

namespace pcad {
    namespace rtlir {
        class instance;
    }
}

#include "module.h++"
#include <pcad/rtlir/statement.h++>
#include <pcad/util/collection.h++>
#include <memory>
#include <vector>

namespace pcad {
    namespace rtlir {
        /* An instance of a particular module. */
        class instance {
        public:
            typedef std::shared_ptr<instance> ptr;

        private:
            const std::string _name;
            const std::shared_ptr<rtlir::module> _module;
            const std::vector<rtlir::port_connect_statement::ptr> _connects;

        public:
            instance(
                const std::string& name,
                const std::shared_ptr<module>& module,
                const std::vector<port_connect_statement::ptr>& connects)
            : _name(name),
              _module(module),
              _connects(connects)
            {}

        public:
            const decltype(_name)& name(void) const { return _name; }
            const std::shared_ptr<rtlir::module> module(void) const { return _module; }
            const std::vector<port_connect_statement::ptr>& port_connections(void) const
            { return _connects; }
        };
    }
}

#endif
