// See LICENSE for details

#ifndef PCAD__RTLIR__INTSANCE_HXX
#define PCAD__RTLIR__INTSANCE_HXX

namespace pcad {
    namespace rtlir {
        class instance;
    }
}

#include "module.h++"
#include <memory>

namespace pcad {
    namespace rtlir {
        /* An instance of a particular module. */
        class instance {
        public:
            typedef std::shared_ptr<instance> ptr;

        private:
            const std::string _name;
            const std::weak_ptr<module> _module;

        public:
            instance(const std::string& name, const std::shared_ptr<module> module)
            : _name(name),
              _module(module)
            {}
        };
    }
}

#endif
