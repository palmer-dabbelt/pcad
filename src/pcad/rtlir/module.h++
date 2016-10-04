// See LICENSE for details

#ifndef PCAD__RTLIR__MODULE_HXX
#define PCAD__RTLIR__MODULE_HXX

#include <memory>
#include <string>

namespace pcad {
    namespace rtlir {
        /* This does moduley things, but in RTL. */
        class module {
        public:
            typedef std::shared_ptr<module> ptr;

        private:
            const std::string _name;

        public:
            module(const decltype(_name)& name)
            : _name(name)
            {}

        public:
            virtual const decltype(_name)& name(void) const { return _name; }
        };
    }
}

#endif
