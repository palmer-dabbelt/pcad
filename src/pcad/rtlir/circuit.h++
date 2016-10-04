// See LICENSE for details

#ifndef PCAD__RTLIR__CIRCUIT_HXX
#define PCAD__RTLIR__CIRCUIT_HXX

#include "module.h++"
#include <memory>
#include <vector>

namespace pcad {
    namespace rtlir {
        /* A rtlir-level circuit, which is really just a pointer to a top level
         * design. */
        class circuit {
        public:
            typedef std::shared_ptr<circuit> ptr;

        private:
            const std::vector<module::ptr> _modules;
            const module::ptr _top;

        public:
            circuit(const module::ptr& top)
            : _modules({top}),
              _top(top)
            {}

        public:
            const decltype(_modules)& modules(void) const { return _modules; }
            const decltype(_top)& top(void) const { return _top; }
        };
    }
}

#endif
