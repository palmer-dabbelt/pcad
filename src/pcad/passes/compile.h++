// See LICENSE for details

#ifndef PCAD__PASSES__COMPILE_HXX
#define PCAD__PASSES__COMPILE_HXX

#include <pcad/rtlir/circuit.h++>
#include <pcad/netlist/macro.h++>
#include <vector>

namespace pcad {
    namespace passes {
        /* The PCAD macro compiler.  Takes a set of macros called "to_compile"
         * and compiles them using a library of already implemented macros
         * (called "compile_to"), yielding another set of macros. */
        std::vector<rtlir::circuit::ptr> compile(
            const std::vector<netlist::macro::ptr>& to_compile,
            const std::vector<netlist::macro::ptr>& compile_to);
    }
}

#endif
