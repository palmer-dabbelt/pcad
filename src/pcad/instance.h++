// See LICENSE for details

#ifndef PCAD__INSTANCE_HXX
#define PCAD__INSTANCE_HXX

namespace pcad {
    class instance;
}

#include "module.h++"
#include "statement.h++"
#include <libjson/ofstream.h++>
#include <memory>

namespace pcad {
    /* A single instance of a module.  Since this has to support Verilog,
     * assignment to instances actually arrive via the */
    class instance {
    public:
        typedef std::shared_ptr<instance> ptr;

    private:
        const std::string _name;
        const std::shared_ptr<module> _module;
        const std::vector<port::ptr> _ports;
        const std::vector<statement::ptr> _assignments;

    public:
        instance(const std::string& name,
                 const decltype(_module)& module,
                 const std::vector<port::ptr>& ports,
                 const std::vector<statement::ptr> assignments)
        : _name(name),
          _module(module),
          _ports(ports),
          _assignments(assignments)
        {
        }

    public:
        const decltype(_ports)& ports(void) const { return _ports; }
        const decltype(_assignments)& assignments(void) const { return _assignments; }

    public:
        virtual void dump(libjson::ofstream& os) const;
    };
}

#endif
