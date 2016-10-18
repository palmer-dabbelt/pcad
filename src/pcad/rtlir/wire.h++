// See LICENSE for details

#ifndef PCAD__RTLIR__WIRE_HXX
#define PCAD__RTLIR__WIRE_HXX

#include <memory>

namespace pcad {
    namespace rtlir {
        class wire {
        public:
            typedef std::shared_ptr<wire> ptr;

        private:
            const std::string _name;
            const int _width;

        public:
            wire(const decltype(_name)& name,
                 const decltype(_width)& width
            ) : _name(name),
                _width(width)
            {}

        public:
            const decltype(_name)& name(void) const { return _name; }
            const decltype(_width)& width(void) const { return _width; }
        };
    }
}

#endif
