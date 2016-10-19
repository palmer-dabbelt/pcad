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

        class blackbox: public macro {
        public:
            typedef std::shared_ptr<blackbox> ptr;

        public:
            blackbox(const std::string& name)
            : macro(name)
            {}
        };

        /* Memory macros map to things like SRAMs. */
        class memory_macro_port {
        public:
            typedef std::shared_ptr<memory_macro_port> ptr;

        private:
            util::option<std::string> _clock_port_name;
            util::option<int> _mask_gran;
            util::option<std::string> _output_port_name;
            util::option<std::string> _input_port_name;
            util::option<std::string> _address_port_name;
            util::option<std::string> _mask_port_name;
            util::option<std::string> _chip_enable_port_name;
            util::option<std::string> _write_enable_port_name;
            util::option<int> _bit_width;
            util::option<int> _word_depth;

        public:
            memory_macro_port(
                const decltype(_clock_port_name)& clock_port_name,
                const decltype(_mask_gran)& mask_gran,
                const decltype(_output_port_name)& output_port_name,
                const decltype(_input_port_name)& input_port_name,
                const decltype(_address_port_name)& address_port_name,
                const decltype(_mask_port_name)& mask_port_name,
                const decltype(_chip_enable_port_name)& chip_enable_port_name,
                const decltype(_write_enable_port_name)& write_enable_port_name,
                const decltype(_bit_width)& bit_width,
                const decltype(_word_depth)& word_depth
            )
            : _clock_port_name(clock_port_name),
              _mask_gran(mask_gran),
              _output_port_name(output_port_name),
              _input_port_name(input_port_name),
              _address_port_name(address_port_name),
              _mask_port_name(mask_port_name),
              _chip_enable_port_name(chip_enable_port_name),
              _write_enable_port_name(write_enable_port_name),
              _bit_width(bit_width),
              _word_depth(word_depth)
            {}

        public:
            const decltype(_clock_port_name)& clock_port_name(void) const { return _clock_port_name; }
            const decltype(_mask_gran)& mask_gran(void) const { return _mask_gran; }
            const decltype(_output_port_name)& output_port_name(void) const { return _output_port_name; }
            const decltype(_input_port_name)& input_port_name(void) const { return _input_port_name; }
            const decltype(_address_port_name)& address_port_name(void) const { return _address_port_name; }
            const decltype(_mask_port_name)& mask_port_name(void) const { return _mask_port_name; }
            const decltype(_chip_enable_port_name)& chip_enable_port_name(void) const { return _chip_enable_port_name; }
            const decltype(_write_enable_port_name)& write_enable_port_name(void) const { return _write_enable_port_name; }
            const decltype(_bit_width)& bit_width(void) const { return _bit_width; }
            const decltype(_word_depth)& word_depth(void) const { return _word_depth; }

        public:
            const rtlir::port::ptr clock_port(void) const {
                return std::make_shared<rtlir::port>(
                    clock_port_name().data(),
                    1,
                    rtlir::port_direction::INPUT
                );
            }

            const rtlir::port::ptr output_port(void) const {
                if (output_port_name().valid())
                    return std::make_shared<rtlir::port>(
                        output_port_name().data(),
                        bit_width().data(),
                        rtlir::port_direction::OUTPUT
                    );
                else
                    return nullptr;
            }

            const rtlir::port::ptr input_port(void) const {
                if (input_port_name().valid())
                    return std::make_shared<rtlir::port>(
                        input_port_name().data(),
                        bit_width().data(),
                        rtlir::port_direction::INPUT
                    );
                else
                    return nullptr;
            }
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
            const decltype(_ports)& mem_ports(void) const { return _ports; }
        };

        class memory_blackbox: public memory_macro {
        public:
            typedef std::shared_ptr<memory_blackbox> ptr;

        public:
            memory_blackbox(const memory_macro::ptr& parent)
            : memory_macro(
                parent->name(),
                parent->depth(),
                parent->width(),
                parent->mem_ports()
            )
            {}
        };
    }
}

#endif
