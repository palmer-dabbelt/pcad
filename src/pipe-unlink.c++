// See LICENSE for details

#include <pcad/open.h++>
#include <pcad/serialize/verilog/dump.h++>
#include <tclap/CmdLine.h>

int main(int argc, const char **argv)
{
    try {
        TCLAP::CmdLine cmd(
            "Dumps any circuit as Verilog\n",
            ' ',
            "0");

        TCLAP::ValueArg<std::string> input("i",
                                           "input",
                                           "A circuit, in any format pcad supports",
                                           true,
                                           "",
                                           "Top-in.v");
        cmd.add(input);

        TCLAP::ValueArg<std::string> output("o",
                                            "output",
                                            "",
                                            true,
                                            "",
                                            "Top-out.v");
        cmd.add(output);

        TCLAP::MultiArg<std::string> split_at("s",
                                              "split",
                                              "",
                                              true
                                              "");
        cmd.add(split_at);

        cmd.parse(argc, argv);

        auto circuit = pcad::open_circuit(input.getValue());
        {
            std::ofstream os(output.getValue());
            pcad::serialize::verilog::dump(os, circuit);
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
