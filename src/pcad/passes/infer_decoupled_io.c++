// See LICENSE for details

#include "infer_decoupled_io.h++"
using namespace pcad;

/* http://stackoverflow.com/questions/874134/find-if-string-ends-with-another-string-in-c */
static inline
bool hasEnding (std::string const &fullString, std::string const &ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}

static inline
bool hasBeginning (std::string const &fullString, std::string const &beginning) {
    if (fullString.length() >= beginning.length()) {
        return (0 == fullString.compare (0, beginning.length(), beginning));
    } else {
        return false;
    }
}

std::vector<passes::decoupled_io::ptr>
passes::infer_decoupled_io(const hdlast::module::ptr& module)
{
    auto out = decltype(passes::infer_decoupled_io(module))();

    for (const auto& port: module->ports()) {
        if (hasEnding(port->name(), "_valid") == false)
            continue;

        bool has_ready = false;
        bool has_valid = false;
        auto ports = std::vector<hdlast::port::ptr>();
        auto base = port->name().substr(0, port->name().size() - 6);
        auto direction = hdlast::port_direction::INPUT;
        for (const auto& candidate: module->ports()) {
            if (candidate->name() == base + "_ready")
                has_ready = true;
            if (candidate->name() == base + "_valid") {
                has_valid = true;
                direction = candidate->direction();
            }

            if (hasBeginning(candidate->name(), base + "_bits_"))
                ports.push_back(candidate);
            if (candidate->name() == (base + "_bits"))
                ports.push_back(candidate);
        }

        if (has_ready == false)
            continue;
        if (has_valid == false)
            continue;
        if (ports.size() == 0)
            continue;

        out.push_back(std::make_shared<decoupled_io>(base, ports, direction));
    }

    return out;
}
