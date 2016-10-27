// See LICENSE for details

#ifndef PCAD__HDLAST__WIRE_HXX
#define PCAD__HDLAST__WIRE_HXX

#include <memory>
#include <string>

namespace pcad {
    namespace hdlast {
        /* This class actually stores both Verilog wires and regs, since they're
         * pretty much the same thing. */
        class wire {
        public:
            typedef std::shared_ptr<wire> ptr;

        public:
            const std::string _name;
            const long _width;
            const long _depth;

        public:
            wire(const std::string& name, const long& width);
            wire(const std::string& name, const long& width, const long& depth);
            virtual ~wire(void) {}

        public:
            const decltype(_name)& name(void) const { return _name; }
            const decltype(_width)& width(void) const { return _width; }
            const decltype(_depth)& depth(void) const { return _depth; }

            virtual std::string as_string(void) const { return name(); }
        };
    }
}

#endif
