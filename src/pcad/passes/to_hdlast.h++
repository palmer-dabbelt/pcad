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
        hdlast::instance::ptr to_hdlast(const rtlir::instance::ptr& p);
        hdlast::wire::ptr to_hdlast(const rtlir::wire::ptr& p);
        hdlast::statement::ptr to_hdlast(const rtlir::statement::ptr& p);
        hdlast::wire_statement::ptr to_hdlast(const rtlir::wire_statement::ptr& p);
        hdlast::assign_statement::ptr to_hdlast(const rtlir::connect_statement::ptr& p);
    }
}

#endif
