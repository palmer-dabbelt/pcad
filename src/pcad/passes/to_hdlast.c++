// See LICENSE for details

#include "to_hdlast.h++"
#include "randomize_x_assign.h++"
#include <pcad/hdlast/blackbox.h++>
#include <pcad/hdlast/reg.h++>
#include <pcad/netlist/macro.h++>
#include <pcad/util/assert.h++>
#include <pcad/util/collection.h++>
#include <simple_match/simple_match.hpp>
#include <cmath>
#include <iostream>
using namespace pcad;
using namespace simple_match;
using namespace simple_match::placeholders;

hdlast::circuit::ptr passes::to_hdlast(const rtlir::circuit::ptr& circuit, bool syn_flops)
{
    std::vector<hdlast::module::ptr> compiled_modules;

    for (const auto& m: circuit->modules())
        compiled_modules.push_back(to_hdlast(m, syn_flops));

    return std::make_shared<hdlast::circuit>(
        compiled_modules,
        circuit->top()->name()
    );
}

hdlast::module::ptr passes::to_hdlast(const rtlir::module::ptr& module, bool syn_flops)
{
    auto out = match(module,
        someptr<netlist::memory_macro>(), [&](auto m) {
            /* The whole point of a memory is to store stuff.  Here's where all
             * the bits live. */
            auto memory = std::make_shared<hdlast::reg>("mem", m->width(), m->depth());
            auto wires = std::vector<hdlast::wire::ptr>{memory};

            /* Generates all the ports (and the logic associated with each
             * port) for the memory. */
            auto logic = std::vector<hdlast::statement::ptr>();
            auto ports = std::vector<hdlast::port::ptr>();
            for (size_t i = 0; i < m->mem_ports().size(); ++i) {
                auto p = m->mem_ports()[i];
                auto clock = std::make_shared<hdlast::port>(
                    p->clock_port_name().data(),
                    1,
                    hdlast::port_direction::INPUT
                );
                ports.push_back(clock);

                auto output = [&]() -> hdlast::wire::ptr {
                    if (p->output_port_name().valid() == true) {
                        auto op = std::make_shared<hdlast::port>(
                            p->output_port_name().data(),
                            m->width(),
                            hdlast::port_direction::OUTPUT
                        );
                        ports.push_back(op);
                        return op;
                    } else {
                        auto op = std::make_shared<hdlast::wire>(
                            "output_" + std::to_string(i),
                            m->width()
                        );
                        wires.push_back(op);
                        return op;
                    }
                }();

                auto input = [&]() -> hdlast::wire::ptr {
                    if (p->input_port_name().valid() == true) {
                        auto ip = std::make_shared<hdlast::port>(
                            p->input_port_name().data(),
                            m->width(),
                            hdlast::port_direction::INPUT
                        );
                        ports.push_back(ip);
                        return ip;
                    } else {
                        auto ip = std::make_shared<hdlast::wire>(
                            "input_" + std::to_string(i),
                            m->width()
                        );
                        wires.push_back(ip);
                        return ip;
                    }
                }();

                auto write_enable = [&]() -> hdlast::wire::ptr {
                    if (p->write_enable_port_name().valid() == true) {
                        auto wep = std::make_shared<hdlast::port>(
                            p->write_enable_port_name().data(),
                            1,
                            hdlast::port_direction::INPUT
                        );
                        ports.push_back(wep);
                        return wep;
                    } else {
                        auto wep = std::make_shared<hdlast::wire>(
                            "write_enable_" + std::to_string(i),
                            1
                        );
                        wires.push_back(wep);
                        logic.push_back(
                            std::make_shared<hdlast::assign_statement>(
                                std::make_shared<hdlast::wire_statement>(wep),
                                std::make_shared<hdlast::unop_statement>(
                                    hdlast::unop_statement::op::NOT,
                                    std::make_shared<hdlast::wire_statement>(
                                        std::make_shared<hdlast::literal>(0, 1)
                                    )
                                )
                            )
                        );
                        return wep;
                    }
                }();

                auto mask_gran = p->mask_gran().data(1);
                auto mask = [&]() -> hdlast::wire::ptr {
                    if (p->mask_gran().valid() == true) {
                        auto mp = std::make_shared<hdlast::port>(
                            p->mask_port_name().data(),
                            m->width() / mask_gran,
                            hdlast::port_direction::INPUT
                        );
                        ports.push_back(mp);
                        return mp;
                    } else {
                        auto mp = std::make_shared<hdlast::wire>(
                            "mask_" + std::to_string(i),
                             m->width() / mask_gran
                        );
                        wires.push_back(mp);
                        logic.push_back(
                            std::make_shared<hdlast::assign_statement>(
                                std::make_shared<hdlast::wire_statement>(mp),
                                std::make_shared<hdlast::unop_statement>(
                                    hdlast::unop_statement::op::NOT,
                                    std::make_shared<hdlast::wire_statement>(
                                        std::make_shared<hdlast::literal>(0, m->width())
                                    )
                                )
                            )
                        );
                        return mp;
                    }
                }();

                auto address = std::make_shared<hdlast::port>(
                    p->address_port_name().data(),
                    std::ceil(std::log2(m->depth())),
                    hdlast::port_direction::INPUT
                );
                ports.push_back(address);

                auto enable = std::make_shared<hdlast::port>(
                    p->chip_enable_port_name().data(),
                    1,
                    hdlast::port_direction::INPUT
                );
                ports.push_back(enable);

                auto read_data = std::make_shared<hdlast::wire>(
                    "read_data_" + std::to_string(i),
                    m->width()
                );
                wires.push_back(read_data);

                auto address_buffer = std::make_shared<hdlast::reg>(
                    "address_buffer_" + std::to_string(i),
                    std::ceil(std::log2(m->depth()))
                );
                wires.push_back(address_buffer);

                auto read_block = std::vector<hdlast::statement::ptr>();
                logic.push_back(
                    std::make_shared<hdlast::assign_statement>(
                        std::make_shared<hdlast::wire_statement>(read_data),
                        std::make_shared<hdlast::index_statement>(
                            std::make_shared<hdlast::wire_statement>(memory),
                            std::make_shared<hdlast::wire_statement>(address_buffer)
                        )
                    )
                );
                read_block.push_back(
                    std::make_shared<hdlast::assign_statement>(
                        std::make_shared<hdlast::wire_statement>(address_buffer),
                        std::make_shared<hdlast::wire_statement>(address)
                    )
                );

                logic.push_back(
                    std::make_shared<hdlast::always_statement>(
                        std::make_shared<hdlast::posedge_statement>(clock),
                        read_block
                    )
                );
                if (syn_flops == false) {
                    for (const auto& ra: randomize_x_assign(output, read_data))
                        logic.push_back(ra);
                } else {
                    logic.push_back(
                        std::make_shared<hdlast::assign_statement>(
                            std::make_shared<hdlast::wire_statement>(output),
                            std::make_shared<hdlast::wire_statement>(read_data)
                        )
                    );
                }

                auto write_block = std::vector<hdlast::statement::ptr>();
                for (auto i = 0; i < m->width(); ++i) {
                    auto mask_index = std::make_shared<hdlast::wire_statement>(
                        std::make_shared<hdlast::literal>(
                            i / mask_gran,
                            std::ceil(std::log2(m->width()))
                        )
                    );

                    auto mem_index = std::make_shared<hdlast::wire_statement>(
                        std::make_shared<hdlast::literal>(
                            i,
                            std::ceil(std::log2(m->width()))
                        )
                    );

                    write_block.push_back(
                        std::make_shared<hdlast::if_statement>(
                            std::make_shared<hdlast::biop_statement>(
                                hdlast::biop_statement::op::ANDAND,
                                std::make_shared<hdlast::biop_statement>(
                                    hdlast::biop_statement::op::ANDAND,
                                    std::make_shared<hdlast::wire_statement>(enable),
                                    std::make_shared<hdlast::wire_statement>(write_enable)
                                ),
                                std::make_shared<hdlast::index_statement>(
                                    std::make_shared<hdlast::wire_statement>(mask),
                                    mask_index
                                )
                            ),
                            std::vector<hdlast::statement::ptr>{
                                std::make_shared<hdlast::assign_statement>(
                                    std::make_shared<hdlast::index_statement>(
                                        std::make_shared<hdlast::index_statement>(
                                            std::make_shared<hdlast::wire_statement>(memory),
                                            std::make_shared<hdlast::wire_statement>(address)
                                        ),
                                        mem_index
                                    ),
                                    std::make_shared<hdlast::index_statement>(
                                        std::make_shared<hdlast::wire_statement>(input),
                                        mem_index
                                    )
                                )
                            },
                            std::vector<hdlast::statement::ptr>()
                        )
                    );
                }

                if (p->input_port_name().valid() == true) {
                    logic.push_back(
                        std::make_shared<hdlast::always_statement>(
                            std::make_shared<hdlast::posedge_statement>(clock),
                            write_block
                        )
                    );
                }
            }
            auto body = std::make_shared<hdlast::scope>(
                std::make_shared<hdlast::scope>(ports),
                wires
            );

            auto hm = std::make_shared<hdlast::module>(
                m->name(),
                ports,
                body,
                logic,
                std::vector<hdlast::instance::ptr>{}
            );

            return match(m,
                someptr<netlist::memory_blackbox>(), [&](const auto& m) -> hdlast::module::ptr {
                    return std::make_shared<hdlast::blackbox>(hm);
                },
                someptr<netlist::memory_macro>(), [&](const auto& m) {
                    return hm;
                }
            );
        },
        someptr<rtlir::module>(), [&](const auto& m) {
            auto ports = putil::collection::map(
                m->ports(),
                [](const auto& p){ return to_hdlast(p); }
            );
            auto wires = putil::collection::map(
                m->wires(),
                [](const auto& w){ return to_hdlast(w); }
            );
            auto logic = putil::collection::map(
                m->statements(),
                [](const auto& s){ return to_hdlast(s); }
            );
            auto instances = putil::collection::map(
                m->instances(),
                [](const auto& i){ return to_hdlast(i); }
            );

            auto body = std::make_shared<hdlast::scope>(
                std::make_shared<hdlast::scope>(ports),
                wires
            );

            return std::make_shared<hdlast::module>(
                m->name(),
                ports,
                body,
                logic,
                instances
            );
        }
    );

    if (out == nullptr) {
        std::cerr << "Unable to convert rtlir::module to hdlast::module\n";
        abort();
    }

    return out;
}

std::vector<hdlast::module::ptr> passes::to_hdlast(const std::vector<rtlir::module::ptr>& modules, bool syn_flops)
{
    return putil::collection::map(modules, [&](const auto& m){ return to_hdlast(m, syn_flops); });
}

hdlast::port::ptr passes::to_hdlast(const rtlir::port::ptr& p)
{
    return std::make_shared<hdlast::port>(
        p->name(),
        p->width(),
        p->hdlast_direction()
    );
}

hdlast::instance::ptr passes::to_hdlast(const rtlir::instance::ptr& i)
{
    /* FIXME: This is actually broken: I'm making a largely empty macro and I
     * think this is the only place where I'm losing pointer equality for the
     * HDL AST.  I don't really care right now, since I really need to go redo
     * the whole type system. */
    return std::make_shared<hdlast::instance>(
        i->name(),
        std::make_shared<hdlast::module>(
            i->module()->name()
        ),
        putil::collection::map(
            i->module()->ports(),
            [](const auto& p) { return to_hdlast(p); }
        ),
        putil::collection::map(
            i->port_connections(),
            [](const auto& pc) { return to_hdlast(pc); }
        )
    );
}

hdlast::wire::ptr passes::to_hdlast(const rtlir::wire::ptr& w)
{
    return std::make_shared<hdlast::wire>(
        w->name(),
        w->width()
    );
}

hdlast::statement::ptr passes::to_hdlast(const rtlir::statement::ptr& s)
{
    auto out = match(s,
        someptr<rtlir::wire_statement>(), [](const auto& ws) {
            return to_hdlast(ws);
        },
        someptr<rtlir::connect_statement>(), [](const auto& cs) -> hdlast::statement::ptr {
            return to_hdlast(cs);
        },
        someptr<rtlir::port_connect_statement>(), [](const auto& cs) -> hdlast::statement::ptr {
            return to_hdlast(cs);
        },
        someptr<rtlir::cat_statement>(), [](const auto& cs) {
            return to_hdlast(cs);
        },
        someptr<rtlir::slice_statement>(), [](const auto& ss) {
            return to_hdlast(ss);
        },
        someptr<rtlir::literal_statement>(), [](const auto& ls) {
            return to_hdlast(ls);
        },
        someptr<rtlir::binop_statement>(), [](const auto& bs) {
            return to_hdlast(bs);
        },
        otherwise, [&]() -> hdlast::statement::ptr {
            std::cerr << "Unable to convert rtlir::statement to hdlast::statement\n";
#ifndef __clang__
            std::cerr << "  " << typeid(*s).name() << "\n";
#endif
            abort();
            return nullptr;
        }
    );

    if (out == nullptr) {
        std::cerr << "Some sort of hdlast->rtlir conversion failed\n";
#ifndef __clang__
        std::cerr << "  " << typeid(*s).name() << "\n";
#endif
        abort();
    }

    return out;
}

hdlast::wire_statement::ptr passes::to_hdlast(const rtlir::wire_statement::ptr& ws)
{
    return std::make_shared<hdlast::wire_statement>(to_hdlast(ws->wire()));
}

hdlast::wire_statement::ptr passes::to_hdlast(const rtlir::port_statement::ptr& ws)
{
    return std::make_shared<hdlast::wire_statement>(to_hdlast(ws->wire()));
}

hdlast::assign_statement::ptr passes::to_hdlast(const rtlir::connect_statement::ptr& cs)
{
    return std::make_shared<hdlast::assign_statement>(
        to_hdlast(cs->target()),
        to_hdlast(cs->source())
    );
}

hdlast::assign_statement::ptr passes::to_hdlast(const rtlir::port_connect_statement::ptr& a)
{
    auto flip_source = [&]() -> hdlast::statement::ptr {
        switch (a->target()->polarity()) {
        case rtlir::port_polarity::NONE:
            std::cerr << "Connection to non-polar port " << a->target()->port()->name() << "\n";
            abort();
            break;

        case rtlir::port_polarity::POSITIVE_EDGE:
        case rtlir::port_polarity::ACTIVE_HIGH:
            return to_hdlast(a->source());

        case rtlir::port_polarity::NEGATIVE_EDGE:
        case rtlir::port_polarity::ACTIVE_LOW:
            return std::make_shared<hdlast::unop_statement>(hdlast::unop_statement::op::NOT, to_hdlast(a->source()));
        }

        std::cerr << "port_polarity enum overflow\n";
        abort();
        return nullptr;
    }();

    switch (a->target()->direction()) {
    case rtlir::port_direction::INPUT:
        return std::make_shared<hdlast::assign_statement>(
            to_hdlast(a->target()),
            flip_source
        );
    case rtlir::port_direction::OUTPUT:
        return std::make_shared<hdlast::assign_statement>(
            to_hdlast(a->target()),
            flip_source
        );
    }

    std::cerr << "unknown port direction\n";
    abort();
}

hdlast::cat_statement::ptr passes::to_hdlast(const rtlir::cat_statement::ptr& cs)
{
    return std::make_shared<hdlast::cat_statement>(
        putil::collection::reverse(
            putil::collection::map(
                cs->sources(),
                [](const auto& s) { return to_hdlast(s); }
            )
        )
    );
}

hdlast::slice_statement::ptr passes::to_hdlast(const rtlir::slice_statement::ptr& ss)
{
    return std::make_shared<hdlast::slice_statement>(
        to_hdlast(ss->source()),
        to_hdlast(ss->hi()),
        to_hdlast(ss->lo())
    );
}

hdlast::wire_statement::ptr passes::to_hdlast(const rtlir::literal_statement::ptr& ls)
{
    return std::make_shared<hdlast::wire_statement>(
        std::make_shared<hdlast::literal>(
            ls->data()->as_string(),
            ls->data()->width()
        )
    );
}

hdlast::biop_statement::ptr passes::to_hdlast(const rtlir::binop_statement::ptr& p)
{
    switch (p->op()) {
    case rtlir::binary_op::AND:
        return std::make_shared<hdlast::biop_statement>(
            hdlast::biop_statement::op::ANDAND,
            to_hdlast(p->left()),
            to_hdlast(p->right())
        );
    case rtlir::binary_op::OR:
        return std::make_shared<hdlast::biop_statement>(
            hdlast::biop_statement::op::OR,
            to_hdlast(p->left()),
            to_hdlast(p->right())
        );
    case rtlir::binary_op::EQEQ:
        return std::make_shared<hdlast::biop_statement>(
            hdlast::biop_statement::op::EQEQ,
            to_hdlast(p->left()),
            to_hdlast(p->right())
        );
    }

    return nullptr;
}
