// See LICENSE for details

#ifndef PCAD__WIRE_HXX
#define PCAD__WIRE_HXX

#include "serialize/json/ofstream.h++"
#include <memory>
#include <string>

namespace pcad {
    /* This class actually stores both Verilog wires and regs, since they're
     * pretty much the same thing. */
    class wire {
    public:
        typedef std::shared_ptr<wire> ptr;

    private:
        const std::string _name;
        const long _width;
        const long _depth;

    public:
        wire(const std::string& name, const long& width);
        wire(const std::string& name, const long& width, const long& depth);

    public:
        const decltype(_name)& name(void) const { return _name; }
        const decltype(_width)& width(void) const { return _width; }
        const decltype(_depth)& depth(void) const { return _depth; }

    public:
        /* Dumps this wire as various formats. */
        virtual void dump(pcad::serialize::json::ofstream& os) const;
    };
}

#endif
