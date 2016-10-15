// See LICENSE for details

#ifndef PCAD__RTLIR__MODULE_HXX
#define PCAD__RTLIR__MODULE_HXX

namespace pcad {
    namespace rtlir {
        class module;
    }
}

#include "instance.h++"
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

        public:
            module(const decltype(_name)& name)
            : _name(name),
              _instances()
            {}

            module(const decltype(_name)& name,
                   const decltype(_instances)& instances)
            : _name(name),
              _instances(instances)
            {}

        public:
            virtual const decltype(_name)& name(void) const { return _name; }
        };
    }
}

#endif
