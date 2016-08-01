// See LICENSE for details

#include <pcad/open.h++>
#include <pcad/serialize/json/ofstream.h++>
#include <tclap/CmdLine.h>

int main(int argc, const char **argv)
{
    try {
        TCLAP::CmdLine cmd(
            "Lists the ports of a module\n",
            ' ',
            "0");

        TCLAP::ValueArg<std::string> input("i",
                                           "input",
                                           "A circuit, in any format pcad supports",
                                           true,
                                           "",
                                           "Top.v");
        cmd.add(input);

        TCLAP::ValueArg<std::string> output("o",
                                            "output",
                                            "A JSON description of the IOs of this circuit",
                                            true,
                                            "",
                                            "Top.io.json");
        cmd.add(output);

        TCLAP::ValueArg<std::string> top("m",
                                         "module",
                                         "The module to find the IOs of",
                                         true,
                                         "",
                                         "Top");
        cmd.add(top);

        cmd.parse(argc, argv);

        auto circuit = pcad::open_circuit(input.getValue());

        auto top_module = circuit->find_module(top.getValue());
        if (top_module == nullptr) {
            std::cerr << "Unable to find module " << top.getValue() << std::endl;
            return 1;
        }

        {
            pcad::serialize::json::ofstream os(output.getValue());
            os << pcad::serialize::json::stream_marker::BEGIN_STRUCTURE;
            os << pcad::serialize::json::make_pair("name", top.getValue());
            os << pcad::serialize::json::make_pair("ports", pcad::serialize::json::stream_marker::BEGIN_ARRAY);
            for (const auto& port: top_module->ports()) {
                os << pcad::serialize::json::stream_marker::BEGIN_STRUCTURE;
                os << pcad::serialize::json::make_pair("name", port->name());
                os << pcad::serialize::json::make_pair("direction", to_string(port->direction()));
                os << pcad::serialize::json::make_pair("type", "wire");
                os << pcad::serialize::json::make_pair("width", port->width());
                os << pcad::serialize::json::stream_marker::END_STRUCTURE;
            }
            os << pcad::serialize::json::stream_marker::END_ARRAY;
            os << pcad::serialize::json::stream_marker::END_STRUCTURE;
        }

        return 0;
    } catch (TCLAP::ArgException &e) {
        std::cerr << "error: "
                  << e.error()
                  << " for arg "
                  << e.argId()
                  << std::endl;
        return 2;
    }

    return 1;
}
