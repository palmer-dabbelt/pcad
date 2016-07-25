// See LICENSE for details

#ifndef PCAD__SCOPE_HXX
#define PCAD__SCOPE_HXX

#include "port.h++"
#include <memory>
#include <unordered_map>
#include <vector>

namespace pcad {
    class scope {
    public:
        typedef std::shared_ptr<scope> ptr;

    private:
        ptr _parent;
        std::vector<wire::ptr> _vars;

        std::unordered_map<std::string, wire::ptr> _name2var;

    public:
        scope(const std::vector<port::ptr>& ports);
        scope(const ptr& parent);

    public:
        /* Adds a new variable to this scope. */
        void add_var(const wire::ptr& wire);

        /* Finds a wire in this scope. */
        wire::ptr lookup(const std::string& name) const;
    };
}

#endif
