// See LICENSE for details

#include "to_hdlast.h++"
#include <pcad/netlist/macro.h++>
#include <pcad/util/collection.h++>
#include <simple_match/simple_match.hpp>
#include <cmath>
#include <iostream>
using namespace pcad;
using namespace simple_match;
using namespace simple_match::placeholders;

hdlast::circuit::ptr passes::to_hdlast(const rtlir::circuit::ptr& circuit)
{
    std::vector<hdlast::module::ptr> compiled_modules;

    for (const auto& m: circuit->modules())
        compiled_modules.push_back(to_hdlast(m));

    return std::make_shared<hdlast::circuit>(
        compiled_modules,
        circuit->top()->name()
    );
}

hdlast::module::ptr passes::to_hdlast(const rtlir::module::ptr& module)
{
    auto out = match(module,
        some<netlist::memory_macro>(), [](auto m) {
            /* The whole point of a memory is to store stuff.  Here's where all
             * the bits live. */
            auto memory = std::make_shared<hdlast::wire>("mem", m.width(), m.depth());
            auto wires = std::vector<hdlast::wire::ptr>{memory};

            /* Generates all the ports (and the logic associated with each
             * port) for the memory. */
            auto logic = std::vector<hdlast::statement::ptr>();
            auto ports = std::vector<hdlast::port::ptr>();
            for (size_t i = 0; i < m.ports().size(); ++i) {
                auto p = m.ports()[i];
                auto clock = std::make_shared<hdlast::port>(
                    p->clock_name(),
                    1,
                    hdlast::port_direction::INPUT
                );
                ports.push_back(clock);

                auto output = std::make_shared<hdlast::port>(
                    p->output_port_name(),
                    1,
                    hdlast::port_direction::INPUT
                );
                ports.push_back(output);

                auto input = std::make_shared<hdlast::port>(
                    p->input_port_name(),
                    1,
                    hdlast::port_direction::INPUT
                );
                ports.push_back(input);

                auto mask = std::make_shared<hdlast::port>(
                    p->mask_port_name(),
                    1,
                    hdlast::port_direction::INPUT
                );
                ports.push_back(mask);

                auto address = std::make_shared<hdlast::port>(
                    p->address_port_name(),
                    1,
                    hdlast::port_direction::INPUT
                );
                ports.push_back(address);

                auto enable = std::make_shared<hdlast::port>(
                    p->enable_port_name(),
                    1,
                    hdlast::port_direction::INPUT
                );
                ports.push_back(enable);

                auto read_data = std::make_shared<hdlast::wire>(
                    "read_data_" + std::to_string(i),
                    m.width()
                );
                wires.push_back(read_data);

                auto read_block = std::vector<hdlast::statement::ptr>();
                read_block.push_back(
                    std::make_shared<hdlast::assign_statement>(
                        std::make_shared<hdlast::wire_statement>(read_data),
                        std::make_shared<hdlast::index_statement>(
                            std::make_shared<hdlast::wire_statement>(memory),
                            std::make_shared<hdlast::wire_statement>(address)
                        )
                    )
                );
                read_block.push_back(
                    std::make_shared<hdlast::assign_statement>(
                        std::make_shared<hdlast::wire_statement>(output),
                        std::make_shared<hdlast::wire_statement>(read_data)
                    )
                );

                logic.push_back(
                    std::make_shared<hdlast::always_statement>(
                        std::make_shared<hdlast::posedge_statement>(clock),
                        read_block
                    )
                );

                auto write_block = std::vector<hdlast::statement::ptr>();
                write_block.push_back(
                    std::make_shared<hdlast::if_statement>(
                        std::make_shared<hdlast::wire_statement>(enable),
                        std::vector<hdlast::statement::ptr>{
                            std::make_shared<hdlast::assign_statement>(
                                std::make_shared<hdlast::index_statement>(
                                    std::make_shared<hdlast::wire_statement>(memory),
                                    std::make_shared<hdlast::wire_statement>(address)
                                ),
                                std::make_shared<hdlast::wire_statement>(input)
                            )
                        },
                        std::vector<hdlast::statement::ptr>()
                    )
                );

                logic.push_back(
                    std::make_shared<hdlast::always_statement>(
                        std::make_shared<hdlast::posedge_statement>(clock),
                        write_block
                    )
                );
            }
            auto body = std::make_shared<hdlast::scope>(
                std::make_shared<hdlast::scope>(ports),
                wires
            );

            return std::make_shared<hdlast::module>(
                m.name(),
                ports,
                body,
                logic,
                std::vector<hdlast::instance::ptr>{}
            );
        },
        none(), [&]() {
            std::cerr << "Unknown rtlir type: " << module << "\n";
            abort();
            return hdlast::module::ptr(nullptr);
        }
    );

    if (out == nullptr) {
        std::cerr << "Unable to convert rtlir::module to hdlast::module\n";
        abort();
    }

    return out;
}
