// See LICENSE for details

#ifndef LIBVERILOG__CIRCUIT_HXX
#define LIBVERILOG__CIRCUIT_HXX

#include <pcad/module.h++>
#include <memory>

namespace libverilog {
    /* The top-level class that holds a whole Verilog circuit. */
    class circuit {
    public:
        typedef std::shared_ptr<circuit> ptr;

    public:
        std::vector<pcad::module::ptr> _modules;

    public:
        circuit(const std::vector<pcad::module::ptr>& modules);

    public:
        const decltype(_modules)& modules(void) const { return _modules; }
    };
}

#endif
