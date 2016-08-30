// See LICENSE for details

#include <pcad/open.h++>
#include <pcad/passes/strip.h++>
#include <pcad/serialize/json/ofstream.h++>
#include <tclap/CmdLine.h>

int main(int argc, const char **argv)
{
    try {
        TCLAP::CmdLine cmd(
            "Dumps any circuit as Verilog\n",
            ' ',
            "0");

        TCLAP::MultiArg<std::string> input("i",
                                           "input",
                                           "A circuit, in any format pcad supports",
                                           true,
                                           "Top-in.v");
        cmd.add(input);

        TCLAP::ValueArg<std::string> output("o",
                                            "output",
                                            "",
                                            true,
                                            "",
                                            "Top-out.v");
        cmd.add(output);

        cmd.parse(argc, argv);

        auto modules = pcad::open_modules(input.getValue());

        {
            pcad::serialize::json::ofstream os(output.getValue());
            os << pcad::serialize::json::stream_marker::BEGIN_ARRAY;
            for (const auto& module: modules) {
                os << pcad::serialize::json::stream_marker::BEGIN_STRUCTURE;
                os << pcad::serialize::json::make_pair("module name", module->name());
                os << pcad::serialize::json::stream_marker::END_STRUCTURE;
            }
            os << pcad::serialize::json::stream_marker::END_ARRAY;
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
