// See LICENSE for details

#ifndef PCAD__OPEN_HXX
#define PCAD__OPEN_HXX

#include "hdlast/circuit.h++"
#include "netlist/macro.h++"

namespace pcad {
    /* Figures out what sort of input file this circuit is and then parses it
     * into a HDL AST. */
    hdlast::circuit::ptr
    open_circuit(const std::string& path, const std::string& top);

    hdlast::circuit::ptr
    open_circuit(const std::vector<std::string>& path, const std::string& top);

    /* Opens a set of modules that don't necessairially have to form a
     * well-formed circuit. */
    std::vector<hdlast::module::ptr>
    open_modules(const std::vector<std::string>& path);

    /* Macro list files define sets of macros.  This reads those in from a JSON
     * file format that's PCAD specific. */
    std::vector<netlist::macro::ptr>
    open_macros(const std::string& path);
}

#endif
