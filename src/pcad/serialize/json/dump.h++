// See LICNESE for details

#ifndef PCAD__SERIALIZE__JSON__DUMP_HXX
#define PCAD__SERIALIZE__JSON__DUMP_HXX

#include <pcad/hdlast/circuit.h++>
#include "ofstream.h++"

namespace pcad {
    namespace serialize {
        namespace json {
            /* Writes a circuit out to a JSON stream, or writes any of the
             * various AST components. */
            void dump(ofstream& out,
                      const hdlast::circuit::ptr& circuit,
                      const std::string& top);

            /* These are used internally by the top-level dumper, but might
             * also be useful. */
            void dump(ofstream& out, const hdlast::module::ptr& module);
            void dump(ofstream& out, const hdlast::wire::ptr& wire);
            void dump(ofstream& out, const hdlast::statement::ptr& statement);
            void dump(ofstream& out, const hdlast::instance::ptr& instance);
        }
    }
}

#endif
