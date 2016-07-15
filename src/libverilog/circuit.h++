// See LICENSE for details

#ifndef LIBVERILOG__CIRCUIT_HXX
#define LIBVERILOG__CIRCUIT_HXX

#include <memory>
#include "module.h++"

namespace libverilog {
    /* The top-level class that holds a whole Verilog circuit. */
    class circuit {
    public:
        typedef std::shared_ptr<circuit> ptr;

    public:
        std::vector<module::ptr> _modules;

    public:
        circuit(const std::vector<module::ptr>& modules);

    public:
        const decltype(_modules)& modules(void) const { return _modules; }
    };
}

#endif
