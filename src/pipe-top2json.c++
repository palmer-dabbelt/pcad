// See LICENSE for details

#include <pcad/circuit.h++>
#include <tclap/CmdLine.h>
#include <fstream>

int main(int argc, const char **argv)
{
    try {
        TCLAP::CmdLine cmd(
            "pcad-pipe-top2json: Lists the top-level IOs of a circuit\n",
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

        TCLAP::ValueArg<std::string> top("t",
                                         "top",
                                         "The module to find the IOs of",
                                         true,
                                         "",
                                         "Top");
        cmd.add(top);

        cmd.parse(argc, argv);

        auto i = pcad::circuit::read_file(input.getValue(), top.getValue());

        auto t = i->find_module(top.getValue());
        if (t == nullptr) {
            std::cerr << "Unable to find module " << top.getValue() << std::endl;
            return 1;
        }

        {
            std::ofstream os(output.getValue());
            os << "{\n"
               << "  \"name\": \"" << t->name() << "\",\n"
               << "  \"ports\": [\n";

            for (const auto& port: t->ports()) {
                os << "    {\n"
                   << "      \"name\": \"" << port->name() << "\",\n"
                   << "      \"direction\": \"" << pcad::to_string(port->direction()) << "\",\n"
                   << "      \"type\": \"wire\",\n"
                   << "      \"width\": \"" << std::to_string(port->width()) << "\"\n"
                   << "    },\n";
            }

            os << "  ]\n"
               << "}\n";
            os.close();
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
