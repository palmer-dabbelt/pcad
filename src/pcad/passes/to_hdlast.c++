// See LICENSE for details

#include "to_hdlast.h++"
#include <pcad/netlist/macro.h++>
#include <pcad/util/collection.h++>
#include <simple_match/simple_match.hpp>
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
            auto ports = 
                putil::collection::flatten(
                    putil::collection::map(
                        m.ports(),
                        [](auto port){ return to_hdlast(port); }
                    )
                );
            auto memory = std::make_shared<hdlast::wire>("mem", m.width(), m.depth());
            auto body = std::make_shared<hdlast::scope>(
                std::make_shared<hdlast::scope>(ports),
                std::vector<hdlast::wire::ptr>{memory}
            );

            return std::make_shared<hdlast::module>(
                m.name(),
                ports,
                body,
                std::vector<hdlast::statement::ptr>{},
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

std::vector<hdlast::port::ptr> passes::to_hdlast(const netlist::memory_macro_port::ptr& p)
{
    return std::vector<hdlast::port::ptr>();
}
