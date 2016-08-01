// See LICENSE for details

#ifndef PCAD__HDLAST__CIRCUIT_HXX
#define PCAD__HDLAST__CIRCUIT_HXX

#include "module.h++"
#include <unordered_map>
#include <memory>
#include <string>

namespace pcad {
    namespace hdlast {
        /* A circuit: the top level container of a bunch of modules. */
        class circuit {
        public:
            typedef std::shared_ptr<circuit> ptr;

        private:
            std::vector<module::ptr> _modules;
            std::unordered_map<std::string, module::ptr> _name2module;

        public:
            circuit(const std::vector<module::ptr>& modules);

        public:
            const decltype(_modules)& modules(void) const { return _modules; }

        public:
            /* Searches for a module by name. */
            module::ptr find_module(const std::string& name) const;
        };
    }
}

#endif
