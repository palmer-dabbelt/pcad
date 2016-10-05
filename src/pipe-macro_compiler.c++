// See LICENSE for details

#include <pcad/open.h++>
#include <pcad/passes/compile.h++>
#include <pcad/passes/link.h++>
#include <pcad/serialize/verilog/dump.h++>
#include <pcad/util/collection.h++>
#include <tclap/CmdLine.h>

int main(int argc, const char **argv)
{
    try {
        TCLAP::CmdLine cmd(
            "Compiles arbitrary macros to a given library\n",
            ' ',
            "0");

        TCLAP::ValueArg<std::string> list("m",
                                          "macro-list",
                                          "The set of macros to compile",
                                          true,
                                          "",
                                          "Top.macros.json");
        cmd.add(list);

        TCLAP::MultiArg<std::string> lib("l",
                                         "library",
                                         "The set of macros that have blackbox instances",
                                         false,
                                         "");
        cmd.add(lib);

        TCLAP::ValueArg<std::string> verilog("v",
                                             "verilog",
                                             "Verilog output",
                                             true,
                                             "",
                                             "Top.macros.v");
        cmd.add(verilog);

        cmd.parse(argc, argv);

        auto to_compile = pcad::open_macros(list.getValue());
        auto compile_to = putil::collection::flatten(
            putil::collection::map(
                lib.getValue(),
                [](auto s) { return pcad::open_macros(s); }
            )
        );
        auto compiled = pcad::passes::compile(to_compile, {compile_to});
        auto flattened = pcad::passes::link(compiled);

        std::ofstream os(verilog.getValue());
        pcad::serialize::verilog::dump(os, flattened);

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
