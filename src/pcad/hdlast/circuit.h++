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
            module::ptr _top;

        public:
            circuit(const std::vector<module::ptr>& modules,
                    const std::string& top_name);

            circuit(const std::vector<module::ptr>& modules,
                    const module::ptr& top);

        public:
            const decltype(_modules)& modules(void) const { return _modules; }
            const decltype(_top)& top(void) const { return _top; }

        public:
            /* Searches for a module by name. */
            module::ptr find_module(const std::string& name) const;
        };
    }
}

#endif
