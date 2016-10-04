// See LICENSE for details

#ifndef PCAD__NETLIST__MODULE_HXX
#define PCAD__NETLIST__MODULE_HXX

#include <pcad/rtlir/module.h++>

namespace pcad {
    namespace netlist {
        /* Modules have defined boundries (ports). */
        class module: public rtlir::module {
        public:
            module(const std::string& name)
            : rtlir::module(name)
            {}
        };
    }
}

#endif
