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

        enum class port_polarity {
            ACTIVE_HIGH, /* For clocks, this also means posedge-triggered. */
            ACTIVE_LOW,  /* For clocks, this also means negedge-triggered. */
        };

        class port: public wire {
        public:
            typedef std::shared_ptr<port> ptr;

        private:
            const enum port_direction _direction;
            const enum port_polarity _polarity;

        public:
            port(
                const std::string& name,
                const size_t& width,
                const enum port_direction& direction,
                const enum port_polarity& polarity
            ): wire(name, width),
               _direction(direction),
               _polarity(polarity)
            {}

        public:
            const decltype(_direction)& direction(void) const { return _direction; }
            const decltype(_polarity)& polarity(void) const { return _polarity; }

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
