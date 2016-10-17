// See LICENSE for details

#ifndef PCAD__PASSES__TO_HDLAST_HXX
#define PCAD__PASSES__TO_HDLAST_HXX

#include <pcad/hdlast/circuit.h++>
#include <pcad/rtlir/circuit.h++>
#include <pcad/rtlir/port.h++>
#include <pcad/netlist/macro.h++>

namespace pcad {
    namespace passes {
        hdlast::circuit::ptr to_hdlast(const rtlir::circuit::ptr& c);
        hdlast::module::ptr to_hdlast(const rtlir::module::ptr& c);
        std::vector<hdlast::port::ptr> to_hdlast(const netlist::memory_macro_port::ptr& p);
        hdlast::port::ptr to_hdlast(const rtlir::port::ptr& p);
    }
}

#endif
