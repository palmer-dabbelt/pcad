// See LICENSE for details

#ifndef PCAD__HDLAST__PORT_HXX
#define PCAD__HDLAST__PORT_HXX

#include "wire.h++"

namespace pcad {
    namespace hdlast {
        /* FIXME: I think GCC can't handle this correctly when it's inside the
         * "port" module. */
        enum class port_direction {
            INPUT,
            OUTPUT
        };

        /* Modules have ports, which are represented here. */
        class port: public wire {
        public:
            typedef std::shared_ptr<port> ptr;

        private:
            const enum port_direction _direction;

        public:
            port(const std::string& name,
                 const long& width,
                 const enum port_direction& dir);

        public:
            const decltype(_direction)& direction(void) const { return _direction; }
        };
    }

}

std::string to_string(const enum pcad::hdlast::port_direction& d);

#endif
