// See LICENSE for details

#include <pcad/open.h++>
#include <simple_match/simple_match.hpp>
#include <tclap/CmdLine.h>
#include <iostream>

using namespace simple_match;
using namespace simple_match::placeholders;

int main(int argc, const char **argv)
{
    try {
        TCLAP::CmdLine cmd(
            "Lists the macros of a certain type\n",
            ' ',
            "0");

        TCLAP::MultiArg<std::string> lib ("l",
                                          "library",
                                          "The set of macros this technology knows about",
                                          true,
                                          "technology.macros.json");
        cmd.add(lib);

        TCLAP::ValueArg<std::string> type("t",
                                          "type",
                                          "The type of the macro we're building",
                                          true,
                                          "",
                                          "sram");
        cmd.add(type);

        cmd.parse(argc, argv);

        auto compile_to = putil::collection::flatten(
            putil::collection::map(
                lib.getValue(),
                [](auto s) { return pcad::open_macros(s); }
            )
        );

        for (const auto& macro: compile_to) {
            match(macro,
                someptr<pcad::netlist::memory_macro>(), [&](auto m) {
                    if (type.getValue() == "sram")
                        std::cout << m->name() << "\n";
                }
            );
        }

        return 0;
    } catch (TCLAP::ArgException &e) {
        std::cerr << "error: "
                  << e.error()
                  << " for arg "
                  << e.argId()
                  << std::endl;
    }

    return 1;
}
