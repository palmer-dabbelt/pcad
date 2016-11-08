// See LICENSE for details

#ifndef PCAD__NETLIST__MACRO_HXX
#define PCAD__NETLIST__MACRO_HXX

#include "module.h++"
#include <pcad/rtlir/port.h++>
#include <pcad/util/assert.h++>
#include <pcad/util/option.h++>
#include <cmath>
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
            const util::option<std::string> _clock_port_name;
            const enum rtlir::port_polarity _clock_port_polarity;
            const util::option<int> _mask_gran;
            const util::option<std::string> _output_port_name;
            const enum rtlir::port_polarity _output_port_polarity;
            const util::option<std::string> _input_port_name;
            const enum rtlir::port_polarity _input_port_polarity;
            const util::option<std::string> _address_port_name;
            const enum rtlir::port_polarity _address_port_polarity;
            const util::option<std::string> _mask_port_name;
            const enum rtlir::port_polarity _mask_port_polarity;
            const util::option<std::string> _chip_enable_port_name;
            const enum rtlir::port_polarity _chip_enable_port_polarity;
            const util::option<std::string> _write_enable_port_name;
            const enum rtlir::port_polarity _write_enable_port_polarity;
            const util::option<std::string> _read_enable_port_name;
            const enum rtlir::port_polarity _read_enable_port_polarity;
            const util::option<int> _bit_width;
            const util::option<int> _word_depth;

        public:
            memory_macro_port(
                const decltype(_clock_port_name)& clock_port_name,
                const decltype(_clock_port_polarity)& clock_port_polarity,
                const decltype(_mask_gran)& mask_gran,
                const decltype(_output_port_name)& output_port_name,
                const decltype(_output_port_polarity)& output_port_polarity,
                const decltype(_input_port_name)& input_port_name,
                const decltype(_input_port_polarity)& input_port_polarity,
                const decltype(_address_port_name)& address_port_name,
                const decltype(_address_port_polarity)& address_port_polarity,
                const decltype(_mask_port_name)& mask_port_name,
                const decltype(_mask_port_polarity)& mask_port_polarity,
                const decltype(_chip_enable_port_name)& chip_enable_port_name,
                const decltype(_chip_enable_port_polarity)& chip_enable_port_polarity,
                const decltype(_write_enable_port_name)& write_enable_port_name,
                const decltype(_write_enable_port_polarity)& write_enable_port_polarity,
                const decltype(_read_enable_port_name)& read_enable_port_name,
                const decltype(_read_enable_port_polarity)& read_enable_port_polarity,
                const decltype(_bit_width)& bit_width,
                const decltype(_word_depth)& word_depth
            )
            : _clock_port_name(clock_port_name),
              _clock_port_polarity(clock_port_polarity),
              _mask_gran(mask_gran),
              _output_port_name(output_port_name),
              _output_port_polarity(output_port_polarity),
              _input_port_name(input_port_name),
              _input_port_polarity(input_port_polarity),
              _address_port_name(address_port_name),
              _address_port_polarity(address_port_polarity),
              _mask_port_name(mask_port_name),
              _mask_port_polarity(mask_port_polarity),
              _chip_enable_port_name(chip_enable_port_name),
              _chip_enable_port_polarity(chip_enable_port_polarity),
              _write_enable_port_name(write_enable_port_name),
              _write_enable_port_polarity(write_enable_port_polarity),
              _read_enable_port_name(read_enable_port_name),
              _read_enable_port_polarity(read_enable_port_polarity),
              _bit_width(bit_width),
              _word_depth(word_depth)
            {
                util::assert(_bit_width.valid(), "memory macro ports must have a width");
                util::assert(_word_depth.valid(), "memory macro ports must have a depth");
            }

        public:
            const decltype(_clock_port_name)& clock_port_name(void) const { return _clock_port_name; }
            const decltype(_mask_gran)& mask_gran(void) const { return _mask_gran; }
            const decltype(_output_port_name)& output_port_name(void) const { return _output_port_name; }
            const decltype(_input_port_name)& input_port_name(void) const { return _input_port_name; }
            const decltype(_address_port_name)& address_port_name(void) const { return _address_port_name; }
            const decltype(_mask_port_name)& mask_port_name(void) const { return _mask_port_name; }
            const decltype(_chip_enable_port_name)& chip_enable_port_name(void) const { return _chip_enable_port_name; }
            const decltype(_write_enable_port_name)& write_enable_port_name(void) const { return _write_enable_port_name; }
            const decltype(_read_enable_port_name)& read_enable_port_name(void) const { return _read_enable_port_name; }
            const decltype(_bit_width)& bit_width(void) const { return _bit_width; }
            const decltype(_word_depth)& word_depth(void) const { return _word_depth; }

        public:
            const rtlir::port::ptr clock_port(void) const {
                return std::make_shared<rtlir::port>(
                    clock_port_name().data(),
                    1,
                    rtlir::port_direction::INPUT,
                    _clock_port_polarity
                );
            }

            const rtlir::port::ptr output_port(void) const {
                if (output_port_name().valid())
                    return std::make_shared<rtlir::port>(
                        output_port_name().data(),
                        bit_width().data(),
                        rtlir::port_direction::OUTPUT,
                        _output_port_polarity
                    );
                else
                    return nullptr;
            }

            const rtlir::port::ptr input_port(void) const {
                if (input_port_name().valid())
                    return std::make_shared<rtlir::port>(
                        input_port_name().data(),
                        bit_width().data(),
                        rtlir::port_direction::INPUT,
                        _input_port_polarity
                    );
                else
                    return nullptr;
            }

            const rtlir::port::ptr address_port(void) const {
                if (address_port_name().valid())
                    return std::make_shared<rtlir::port>(
                        address_port_name().data(),
                        std::ceil(std::log2(word_depth().data())),
                        rtlir::port_direction::INPUT,
                        _address_port_polarity
                    );
                else
                    return nullptr;
            }

            const rtlir::port::ptr mask_port(void) const {
                if (mask_port_name().valid()) {
                    util::assert(bit_width().data() % mask_gran().data() == 0,
                                 "The width of memory macros must be an exact multiple of the mask granularity");
                    return std::make_shared<rtlir::port>(
                        mask_port_name().data(),
                        bit_width().data() / mask_gran().data(),
                        rtlir::port_direction::INPUT,
                        _mask_port_polarity
                    );
                } else
                    return nullptr;
            }
            
            const rtlir::port::ptr chip_enable_port(void) const {
                if (chip_enable_port_name().valid()) {
                    return std::make_shared<rtlir::port>(
                        chip_enable_port_name().data(),
                        1,
                        rtlir::port_direction::INPUT,
                        _chip_enable_port_polarity
                    );
                } else
                    return nullptr;
            }

            const rtlir::port::ptr write_enable_port(void) const {
                if (write_enable_port_name().valid()) {
                    return std::make_shared<rtlir::port>(
                        write_enable_port_name().data(),
                        1,
                        rtlir::port_direction::INPUT,
                        _write_enable_port_polarity
                    );
                } else
                    return nullptr;
            }

            const rtlir::port::ptr read_enable_port(void) const {
                if (read_enable_port_name().valid()) {
                    return std::make_shared<rtlir::port>(
                        read_enable_port_name().data(),
                        1,
                        rtlir::port_direction::INPUT,
                        _read_enable_port_polarity
                    );
                } else
                    return nullptr;
            }

            int effective_mask_gran(void) const {
                if (mask_gran().valid())
                    return mask_gran().data();
                else
                    return bit_width().data();
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
