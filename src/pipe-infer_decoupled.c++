// See LICENSE for details

#include <pcad/circuit.h++>
#include <tclap/CmdLine.h>
#include <libjson/json_stream.h++>

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
            libjson::ofstream os(output.getValue());
            os << libjson::stream_marker::BEGIN_STRUCTURE;
            os << libjson::make_pair("name", t->name());
            os << libjson::make_pair("decoupled io", libjson::stream_marker::BEGIN_ARRAY);

            for (const auto& d: t->infer_decoupled_io()) {
                os << libjson::stream_marker::BEGIN_STRUCTURE;
                os << libjson::make_pair("base", d->base_name());
                os << libjson::make_pair("direction", pcad::to_string(d->direction()));
                os << libjson::make_pair("bits", libjson::stream_marker::BEGIN_ARRAY);
                for (const auto& w: d->ports()) {
                    os << libjson::stream_marker::BEGIN_STRUCTURE;
                    os << libjson::make_pair("name", w->name());
                    os << libjson::make_pair("width", std::to_string(w->width()));
                    os << libjson::stream_marker::END_STRUCTURE;
                }
                os << libjson::stream_marker::END_ARRAY;
                os << libjson::stream_marker::END_STRUCTURE;
            }

            os << libjson::stream_marker::END_ARRAY;
            os << libjson::stream_marker::END_STRUCTURE;
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
