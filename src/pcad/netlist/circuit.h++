// See LICENSE for details

#ifndef PCAD__NETLIST__CIRCUIT_HXX
#define PCAD__NETLIST__CIRCUIT_HXX

#include <memory>
#include <pcad/rtlir/circuit.h++>

namespace pcad {
    namespace netlist {
        /* A netlist-level circuit. */
        class circuit: public rtlir::circuit {
        public:
            typedef std::shared_ptr<circuit> ptr;
        };
    }
}

#endif
