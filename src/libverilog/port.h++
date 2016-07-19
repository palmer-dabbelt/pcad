// See LICENSE for details

#ifndef LIBVERILOG__PORT_HXX
#define LIBVERILOG__PORT_HXX

#include <pcad/wire.h++>

namespace libverilog {
    /* FIXME: I think GCC can't handle this. */
    enum class port_direction {
        INPUT,
        OUTPUT
    };

    /* Modules have ports, which are represented here. */
    class port: public pcad::wire {
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

    std::string to_string(const enum port_direction& d);
}

#endif
