// See LICENSE for details

#include <pcad/open.h++>
#include <pcad/passes/infer_decoupled_io.h++>
#include <pcad/serialize/json/ofstream.h++>
#include <tclap/CmdLine.h>

int main(int argc, const char **argv)
{
    try {
        TCLAP::CmdLine cmd(
            "pcad-pipe-top2json: Lists the top-level IOs of a circuit\n",
            ' ',
            "0");

        TCLAP::MultiArg<std::string> input("i",
                                           "input",
                                           "A circuit, in any format pcad supports",
                                           true,
                                           "");
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

        auto i = pcad::open_circuit(input.getValue());

        auto t = i->find_module(top.getValue());
        if (t == nullptr) {
            std::cerr << "Unable to find module " << top.getValue() << std::endl;
            return 1;
        }

        {
            pcad::serialize::json::ofstream os(output.getValue());
            os << pcad::serialize::json::stream_marker::BEGIN_STRUCTURE;
            os << pcad::serialize::json::make_pair("name", t->name());
            os << pcad::serialize::json::make_pair("decoupled io", pcad::serialize::json::stream_marker::BEGIN_ARRAY);

            for (const auto& d: pcad::passes::infer_decoupled_io(t)) {
                os << pcad::serialize::json::stream_marker::BEGIN_STRUCTURE;
                os << pcad::serialize::json::make_pair("base", d->base_name());
                os << pcad::serialize::json::make_pair("direction", to_string(d->direction()));
                os << pcad::serialize::json::make_pair("bits", pcad::serialize::json::stream_marker::BEGIN_ARRAY);
                for (const auto& w: d->ports()) {
                    os << pcad::serialize::json::stream_marker::BEGIN_STRUCTURE;
                    os << pcad::serialize::json::make_pair("name", w->name());
                    os << pcad::serialize::json::make_pair("width", std::to_string(w->width()));
                    os << pcad::serialize::json::stream_marker::END_STRUCTURE;
                }
                os << pcad::serialize::json::stream_marker::END_ARRAY;
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
