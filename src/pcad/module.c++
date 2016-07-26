// See LICENSE for details

#include "module.h++"
#include <iostream>
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

std::vector<decoupled::ptr> module::infer_decoupled_io(void) const
{
    std::vector<decoupled::ptr> out;

    for (const auto& port: ports()) {
        if (hasEnding(port->name(), "_valid") == false)
            continue;

        bool has_ready = false;
        bool has_valid = false;
        std::vector<port::ptr> ports;
        auto base = port->name().substr(0, port->name().size() - 6);
        auto direction = port_direction::INPUT;
        for (const auto& candidate: this->ports()) {
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

        out.push_back(std::make_shared<decoupled>(base, ports, direction));
    }

    return out;
}

void module::dump(libjson::ofstream& os) const
{
    os << libjson::stream_marker::BEGIN_STRUCTURE;
    os << libjson::make_pair("name", _name);

    os << libjson::make_pair("ports", libjson::stream_marker::BEGIN_ARRAY);
    for (const auto& port: _ports)
        port->dump(os);
    os << libjson::stream_marker::END_ARRAY;

    os << libjson::make_pair("wires", libjson::stream_marker::BEGIN_ARRAY);
    for (const auto& wire: _body->vars())
        wire->dump(os);
    os << libjson::stream_marker::END_ARRAY;

    os << libjson::make_pair("statements", libjson::stream_marker::BEGIN_ARRAY);
    for (const auto& statement: _statements)
        statement->dump(os);
    os << libjson::stream_marker::END_ARRAY;

    os << libjson::stream_marker::END_STRUCTURE;
}
