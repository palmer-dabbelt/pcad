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

module::module(const decltype(_name)& name,
               const decltype(_ports)& ports)
: _name(name),
  _ports(ports)
{
}

std::vector<decoupled::ptr> module::infer_decoupled_io(void) const
{
    std::vector<decoupled::ptr> out;

    for (const auto& port: ports()) {
        if (hasEnding(port->name(), "_valid") == false)
            continue;

        bool has_ready = false;
        std::vector<port::ptr> ports;
        auto base = port->name().substr(0, port->name().size() - 6);
        for (const auto& candidate: this->ports()) {
            if (candidate->name() == base + "_ready")
                has_ready = true;
            if (hasBeginning(candidate->name(), base + "_bits_"))
                ports.push_back(candidate);
        }

        if (has_ready == false)
            continue;

        out.push_back(std::make_shared<decoupled>(base, ports));
    }

    return out;
}
