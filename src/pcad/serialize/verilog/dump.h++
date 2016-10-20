// See LICNESE for details

#ifndef PCAD__SERIALIZE__VERILOG__DUMP_HXX
#define PCAD__SERIALIZE__VERILOG__DUMP_HXX

#include <pcad/hdlast/circuit.h++>
#include <pcad/rtlir/circuit.h++>
#include <fstream>

namespace pcad {
    namespace serialize {
        namespace verilog {
            /* Writes a circuit out as Verilog. */
            void dump(std::ofstream& out,
                      const hdlast::circuit::ptr& circuit);
            void dump(std::ofstream& out,
                      const rtlir::circuit::ptr& circuit);

            /* These are used internally by the top-level dumper, but might
             * also be useful. */
            void dump(std::ofstream& out, const hdlast::module::ptr& module);
            void dump(std::ofstream& out, const hdlast::wire::ptr& wire);
            void dump(std::ofstream& out, const hdlast::statement::ptr& statement, std::string indent);
            void dump(std::ofstream& out, const hdlast::instance::ptr& instance);

            void dump(std::ofstream& out, const std::vector<rtlir::module::ptr>& modules);
            void dump(std::ofstream& out, const std::vector<hdlast::module::ptr>& modules);
        }
    }
}

#endif
