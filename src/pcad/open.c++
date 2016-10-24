// See LICENSE for details

#include "open.h++"
#include "rtlir/port.h++"
#include "serialize/verilog/parser.h++"
#include "util/collection.h++"
#include "util/option.h++"
#include <pson/parser.h++>
#include <simple_match/simple_match.hpp>
#include <iostream>

using namespace pcad;
using namespace simple_match;
using namespace simple_match::placeholders;

pcad::hdlast::circuit::ptr
pcad::open_circuit(const std::string& path, const std::string& top)
{
    return open_circuit(std::vector<std::string>{path}, top);
}

pcad::hdlast::circuit::ptr
pcad::open_circuit(const std::vector<std::string>& names, const std::string& top)
{
    return std::make_shared<hdlast::circuit>(
        open_modules(names),
        top
    );
}

std::vector<hdlast::module::ptr>
pcad::open_modules(const std::vector<std::string>& names)
{
    try {
        auto v = serialize::verilog::parser::read_files(names);
        if (v.size() > 0)
            return v;
    } catch (...) {
        abort();
    }

    std::cerr << "Unable to open circuit by path.  Given files:\n";
    for (const auto& name: names)
        std::cerr << "  " << name << "\n";
    abort();
    return {};
}

std::vector<netlist::macro::ptr> pcad::open_macros(const std::string& path)
{
    auto from_offset = [](const auto& str) {
        if (str.valid() == false)
            return rtlir::port_polarity::ACTIVE_HIGH;
        if (str.data() == "active high")
            return rtlir::port_polarity::ACTIVE_HIGH;
        if (str.data() == "active low")
            return rtlir::port_polarity::ACTIVE_LOW;
        if (str.data() == "positive edge")
            return rtlir::port_polarity::POSITIVE_EDGE;
        if (str.data() == "negative edge")
            return rtlir::port_polarity::NEGATIVE_EDGE;

        std::cerr << "Unknown port polarity: " << str.data() << "\n";
        abort();
    };

    return match(pson::parse_json(path),
        some<pson::tree_array>(), [&](auto macro_jsons) {
            return putil::collection::map(
                macro_jsons.children(),
                [&](auto macro_json) -> netlist::macro::ptr {
                    return match(macro_json,
                        some<pson::tree_object>(), [&](auto macro_json) -> netlist::macro::ptr {
                            auto name = macro_json.template get<std::string>("name");
                            auto depth = macro_json.template get<int>("depth");
                            auto width = macro_json.template get<int>("width");

                            auto ports = macro_json.template map<netlist::memory_macro_port::ptr, pson::tree_object>(
                                "ports",
                                [&](const auto& port_object) {
                                    auto clock_port_name = port_object->template get<std::string>("clock port name");
                                    auto mask_gran = port_object->template get<int>("mask granularity");
                                    auto output_port_name = port_object->template get<std::string>("output port name");
                                    auto input_port_name = port_object->template get<std::string>("input port name");
                                    auto address_port_name = port_object->template get<std::string>("address port name");
                                    auto mask_port_name = port_object->template get<std::string>("mask port name");
                                    auto chip_enable_port_name = port_object->template get<std::string>("chip enable port name");
                                    auto read_enable_port_name = port_object->template get<std::string>("chip enable port name");
                                    auto write_enable_port_name = port_object->template get<std::string>("write enable port name");

                                    auto clock_port_polarity = port_object->template get<std::string>("clock port polarity");
                                    auto output_port_polarity = port_object->template get<std::string>("output port polarity");
                                    auto input_port_polarity = port_object->template get<std::string>("input port polarity");
                                    auto address_port_polarity = port_object->template get<std::string>("address port polarity");
                                    auto mask_port_polarity = port_object->template get<std::string>("mask port polarity");
                                    auto chip_enable_port_polarity = port_object->template get<std::string>("chip enable port polarity");
                                    auto read_enable_port_polarity = port_object->template get<std::string>("chip enable port polarity");
                                    auto write_enable_port_polarity = port_object->template get<std::string>("write enable port polarity");

                                    return std::make_shared<netlist::memory_macro_port>(
                                        util::to_option(clock_port_name),
                                        from_offset(clock_port_polarity),
                                        util::to_option(mask_gran),
                                        util::to_option(output_port_name),
                                        from_offset(output_port_polarity),
                                        util::to_option(input_port_name),
                                        from_offset(input_port_polarity),
                                        util::to_option(address_port_name),
                                        from_offset(address_port_polarity),
                                        util::to_option(mask_port_name),
                                        from_offset(mask_port_polarity),
                                        util::to_option(chip_enable_port_name),
                                        from_offset(chip_enable_port_polarity),
                                        util::to_option(write_enable_port_name),
                                        from_offset(write_enable_port_polarity),
                                        util::to_option(width),
                                        util::to_option(depth)
                                    );
                                }
                            );

                            return std::make_shared<netlist::memory_macro>(
                                name.data(),
                                depth.data(),
                                width.data(),
                                ports
                            );
                        },
                        none(), []() {
                            std::cerr << "macro JSON file should be an array of objects\n";
                            abort();
                            return netlist::macro::ptr(nullptr);
                        }
                    );
                }
            );
        },
        none(), []() {
            std::cerr << "macro JSON file should start with an array\n";
            abort();
            return std::vector<netlist::macro::ptr>();
        }
    );
}
