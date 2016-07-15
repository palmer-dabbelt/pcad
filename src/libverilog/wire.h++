// See LICENSE for details

#ifndef LIBVERILOG__WIRE_HXX
#define LIBVERILOG__WIRE_HXX

#include <memory>
#include <string>

namespace libverilog {
    /* This class actually stores both Verilog wires and regs, since they're
     * pretty much the same thing. */
    class wire {
    public:
        typedef std::shared_ptr<wire> ptr;

    private:
        const std::string _name;
        const long _width;

    public:
        wire(const std::string& name, const long& width);

    public:
        const decltype(_name)& name(void) const { return _name; }
        const decltype(_width)& width(void) const { return _width; }
    };
}

#endif
