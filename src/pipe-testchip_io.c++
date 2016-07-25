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

        TCLAP::ValueArg<std::string> test_v("t",
                                            "testing-verilog",
                                            "A Top-Level wrapper for the test harness",
                                            true,
                                            "",
                                            "TestingTop.v");
        cmd.add(test_v);

        TCLAP::ValueArg<std::string> chip_v("c",
                                            "chip-verilog",
                                            "A Top-Level wrapper for the chip side",
                                            true,
                                            "",
                                            "ChipTop.v");
        cmd.add(chip_v);

        TCLAP::ValueArg<std::string> top("s",
                                         "system-top",
                                         "The module of the non-ASIC RTL",
                                         true,
                                         "",
                                         "Top");
        cmd.add(top);

        cmd.parse(argc, argv);

        auto i = pcad::circuit::read_file(input.getValue(), top.getValue());

        auto t = i->module(top.getValue());
        if (t == nullptr) {
            std::cerr << "Unable to find module " << top.getValue() << std::endl;
            return 1;
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
