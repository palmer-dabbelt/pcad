// See LICENSE for details

#ifndef PCAD__PASSES__LINK_HXX
#define PCAD__PASSES__LINK_HXX

#include <pcad/netlist/circuit.h++>
#include <vector>

namespace pcad {
    namespace passes {
        /* "links" together two circuits -- note that this doesn't actually
         * produce a new circuit because it wasn't given a top name. */
        std::vector<rtlir::module::ptr> link(
            const std::vector<rtlir::circuit::ptr>& circuits
        );

        /* This form of linking produces a circuit because it's given a top. */
        rtlir::circuit::ptr link(
            const std::string& new_top_name,
            const std::vector<rtlir::circuit::ptr>& circuits
        );
                           
    }
}
        

#endif
