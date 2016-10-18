// See LICENES for details

#ifndef PCAD__RTLIR__PORT_HXX
#define PCAD__RTLIR__PORT_HXX

#include "wire.h++"
#include <pcad/hdlast/port.h++>
#include <memory>

namespace pcad {
    namespace rtlir {
        enum class port_direction {
            INPUT,
            OUTPUT
        };

        class port: public wire {
        public:
            typedef std::shared_ptr<port> ptr;

        private:
            const enum port_direction _direction;

        public:
            port(
                const std::string& name,
                const size_t& width,
                const enum port_direction& direction
            ): wire(name, width),
               _direction(direction)
            {}

        public:
            const decltype(_direction)& direction(void) const { return _direction; }

        public:
            hdlast::port_direction hdlast_direction(void) const {
                switch (_direction) {
                case port_direction::INPUT: return hdlast::port_direction::INPUT;
                case port_direction::OUTPUT: return hdlast::port_direction::OUTPUT;
                }

                abort();
                return hdlast::port_direction::INPUT;
            }
        };
    }
}

#endif
