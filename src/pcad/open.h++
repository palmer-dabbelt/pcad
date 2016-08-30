// See LICENSE for details

#ifndef PCAD__OPEN_HXX
#define PCAD__OPEN_HXX

#include "hdlast/circuit.h++"

namespace pcad {
    /* Figures out what sort of input file this circuit is and then parses it
     * into a HDL AST. */
    hdlast::circuit::ptr open_circuit(const std::string& path, const std::string& top);
    hdlast::circuit::ptr open_circuit(const std::vector<std::string>& path, const std::string& top);
}

#endif
