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
            const std::vector<netlist::macro::ptr>& compile_to
        );

        /* If you know your macro sets consist of then one of these functions
         * can be a bit easier to deal with here. */
        rtlir::circuit::ptr compile(
            const netlist::memory_macro::ptr& to_compile,
            const std::vector<netlist::memory_macro::ptr>& compile_to
        );

        rtlir::circuit::ptr compile(
            const netlist::memory_macro::ptr& to_compile,
            const netlist::memory_macro::ptr& compile_to
        );
    }
}

#endif
