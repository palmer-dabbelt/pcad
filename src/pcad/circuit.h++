// See LICENSE for details

#include <libverilog/circuit.h++>
#include <unordered_map>
#include <memory>
#include <string>

namespace pcad {
    /* A circuit: the top level container of a bunch of modules. */
    class circuit {
    public:
        typedef std::shared_ptr<circuit> ptr;

    private:
        libverilog::circuit::ptr _v;
        std::unordered_map<std::string, module::ptr> _name2module;

        module::ptr _top;

    public:
        circuit(const std::string& top,
                const libverilog::circuit::ptr& v);

    public:
        /* Searches for a module by name. */
        module::ptr find_module(const std::string& name) const;

    public:
        static ptr read_file(const std::string& filename, const std::string& top);
    };
}
