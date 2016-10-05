// See LICENSE for details

#ifndef PCAD__NETLIST__MACRO_HXX
#define PCAD__NETLIST__MACRO_HXX

#include "module.h++"
#include <pcad/util/option.h++>
#include <memory>

namespace pcad {
    namespace netlist {
        /* A netlist macro is essentially a black box module, but since there's
         * a finate set of sorts of macros that are the common ones we can know
         * a bit about them.  As such, they're represented a bit differently
         * here. */
        class macro: public module {
        public:
            typedef std::shared_ptr<macro> ptr;

        public:
            macro(const std::string& name)
            : module(name)
            {}
        };

        /* Memory macros map to things like SRAMs. */
        class memory_macro_port {
        public:
            typedef std::shared_ptr<memory_macro_port> ptr;

        private:
            std::string _direction;
            std::string _clock_name;
            util::option<int> _mask_gran;
            util::option<std::string> _output_port_name;
            util::option<std::string> _input_port_name;
            std::string _address_port_name;
            util::option<std::string> _mask_port_name;
            std::string _enable_port_name;

        public:
            memory_macro_port(
                const decltype(_direction)& direction,
                const decltype(_clock_name)& clock_name,
                const decltype(_mask_gran)& mask_gran,
                const decltype(_output_port_name)& output_port_name,
                const decltype(_input_port_name)& input_port_name,
                const decltype(_address_port_name)& address_port_name,
                const decltype(_mask_port_name)& mask_port_name,
                const decltype(_enable_port_name)& enable_port_name
            )
            : _direction(direction),
              _clock_name(clock_name),
              _mask_gran(mask_gran),
              _output_port_name(output_port_name),
              _input_port_name(input_port_name),
              _address_port_name(address_port_name),
              _mask_port_name(mask_port_name),
              _enable_port_name(enable_port_name)
            {}

        public:
            const decltype(_clock_name)& clock_name(void) const { return _clock_name; }
            const decltype(_mask_gran)& mask_gran(void) const { return _mask_gran; }
            const decltype(_output_port_name)& output_port_name(void) const { return _output_port_name; }
            const decltype(_input_port_name)& input_port_name(void) const { return _input_port_name; }
            const decltype(_address_port_name)& address_port_name(void) const { return _address_port_name; }
            const decltype(_mask_port_name)& mask_port_name(void) const { return _mask_port_name; }
            const decltype(_enable_port_name)& enable_port_name(void) const { return _enable_port_name; }
        };

        class memory_macro: public macro {
        public:
            typedef std::shared_ptr<memory_macro> ptr;

        private:
            const int _depth;
            const int _width;
            const std::vector<memory_macro_port::ptr> _ports;

        public:
            memory_macro(
                const std::string& name,
                const decltype(_depth)& depth,
                const decltype(_width)& width,
                const decltype(_ports)& ports
            )
            : macro(name),
              _depth(depth),
              _width(width),
              _ports(ports)
            {}

        public:
            const decltype(_depth)& depth(void) const { return _depth; }
            const decltype(_width)& width(void) const { return _width; }
            const decltype(_ports)& ports(void) const { return _ports; }
        };
    }
}

#endif
