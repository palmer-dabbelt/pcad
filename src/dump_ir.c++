// See LICENSE for details

#include <pcad/circuit.h++>
#include <pcad/serialize/json/ofstream.h++>
#include <tclap/CmdLine.h>

int main(int argc, const char **argv)
{
    try {
        TCLAP::CmdLine cmd(
            "Converts a circuit into a canonical format.  Note this isn't a stable represetation!\n",
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
                                            "A JSON dump of the PCAD IR",
                                            true,
                                            "",
                                            "Top.pcad.json");
        cmd.add(output);

        TCLAP::ValueArg<std::string> top("t",
                                         "top",
                                         "The top-level module in the circuit",
                                         true,
                                         "",
                                         "Top");
        cmd.add(top);

        cmd.parse(argc, argv);

        auto i = pcad::circuit::read_file(input.getValue(), top.getValue());
        pcad::serialize::json::ofstream os(output.getValue());
        i->dump(os);

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
