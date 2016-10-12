// See LICENSE for details

#ifndef PCAD__PASSES__RANDOMIZE_X_HXX
#define PCAD__PASSES__RANDOMIZE_X_HXX

#include <pcad/hdlast/statement.h++>
#include <pcad/hdlast/wire.h++>

namespace pcad {
    namespace passes {
        /* Assigns a wire to another wire, but does a special bitwise sort of
         * asignment that fills in random values instead of X's.  This is
         * necessary for any code that's fed in from Chisel since it can't
         * handle any X's. */
        std::vector<hdlast::statement::ptr> randomize_x_assign(
            const hdlast::wire::ptr& dest,
            const hdlast::wire::ptr& source
        );
    }
}

#endif
