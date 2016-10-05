// See LICENSE for details

#include "link.h++"
#include <iostream>
#include <unordered_set>
using namespace pcad;

std::vector<rtlir::module::ptr> passes::link(const std::vector<rtlir::circuit::ptr>& circuits)
{
    auto out = std::vector<rtlir::module::ptr>();
    auto used_names = std::unordered_set<std::string>();

    for (const auto& circuit: circuits) {
        for (const auto& module: circuit->modules()) {
            auto l = used_names.find(module->name());
            if (l != used_names.end()) {
                std::cerr << "internal error: Duplicate name in circuit for linking: " << module->name() << "\n";
                abort();
            }

            used_names.insert(module->name());
            out.push_back(module);
        }
    }

    return out;
}
