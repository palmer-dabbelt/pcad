// See LICENSE for details

#ifndef PCAD__DECOUPLED_HXX
#define PCAD__DECOUPLED_HXX

#include "port.h++"
#include <memory>
#include <vector>

namespace pcad {
    /* A decoupled interface.  This has an implicit ready/valid pair as well as
     * all the other signals explicited listed.  */
    class decoupled {
    public:
        typedef std::shared_ptr<decoupled> ptr;

    private:
        const std::string _base_name;
        const std::vector<port::ptr> _ports;

    public:
        decoupled(const decltype(_base_name)& base_name,
                  const decltype(_ports)& ports)
        : _base_name(base_name),
          _ports(ports)
        {}

    public:
        const decltype(_ports)& ports(void) const { return _ports; }

    public:
        /* Decoupled IOs all have ports that look like ${base}_bits_${port}.
         * This lists both the base and port names for every Decoupled IO. */
        std::string base_name(void) const { return _base_name; }
        std::string bits_name(const port::ptr& port) const
        { return port->name().substr((_base_name + "_bits_").size()); }

        /* Decoupled IOs are directional.  This cooresponds to the direction of
         * all the _bits ports. */
        enum port_direction direction(void) const;
    };
}

#endif
