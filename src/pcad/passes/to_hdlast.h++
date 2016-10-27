// See LICENSE for details

#ifndef PCAD__PASSES__TO_HDLAST_HXX
#define PCAD__PASSES__TO_HDLAST_HXX

#include <pcad/hdlast/circuit.h++>
#include <pcad/rtlir/circuit.h++>
#include <pcad/rtlir/port.h++>
#include <pcad/netlist/macro.h++>

namespace pcad {
    namespace passes {
        hdlast::circuit::ptr to_hdlast(const rtlir::circuit::ptr& c, bool syn_flops);
        hdlast::module::ptr to_hdlast(const rtlir::module::ptr& c, bool syn_flops);
        std::vector<hdlast::module::ptr> to_hdlast(const std::vector<rtlir::module::ptr>& c, bool syn_flops);
        std::vector<hdlast::port::ptr> to_hdlast(const netlist::memory_macro_port::ptr& p);
        hdlast::port::ptr to_hdlast(const rtlir::port::ptr& p);
        hdlast::instance::ptr to_hdlast(const rtlir::instance::ptr& p);
        hdlast::wire::ptr to_hdlast(const rtlir::wire::ptr& p);
        hdlast::statement::ptr to_hdlast(const rtlir::statement::ptr& p);
        hdlast::wire_statement::ptr to_hdlast(const rtlir::wire_statement::ptr& p);
        hdlast::wire_statement::ptr to_hdlast(const rtlir::port_statement::ptr& p);
        hdlast::assign_statement::ptr to_hdlast(const rtlir::connect_statement::ptr& p);
        hdlast::assign_statement::ptr to_hdlast(const rtlir::port_connect_statement::ptr& p);
        std::vector<hdlast::assign_statement::ptr> to_hdlast(const std::vector<rtlir::port_connect_statement::ptr>& p);
        hdlast::cat_statement::ptr to_hdlast(const rtlir::cat_statement::ptr& p);
        hdlast::slice_statement::ptr to_hdlast(const rtlir::slice_statement::ptr& p);
        hdlast::wire_statement::ptr to_hdlast(const rtlir::literal_statement::ptr& p);
        hdlast::biop_statement::ptr to_hdlast(const rtlir::binop_statement::ptr& p);
        hdlast::unop_statement::ptr to_hdlast(const rtlir::unop_statement::ptr& p);
    }
}

#endif
