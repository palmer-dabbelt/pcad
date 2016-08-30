// See LICENSE for details

#ifndef PCAD__PASSES__STRIP_H
#define PCAD__PASSES__STRIP_H

#include <pcad/hdlast/circuit.h++>
#include <memory>

namespace pcad {
    namespace passes {
        /* Strips a circuit of all the modules that never end up instiantated
         * by said circuit. */
        hdlast::circuit::ptr strip(const hdlast::circuit::ptr& big);
    }
}

#endif
