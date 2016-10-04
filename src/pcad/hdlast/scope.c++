// See LICENSE for details

#include "scope.h++"
using namespace pcad::hdlast;

std::vector<wire::ptr> port2wire(const std::vector<port::ptr>& ports)
{
    auto out = std::vector<wire::ptr>();
    for (const auto& port: ports)
        out.push_back(port);
    return out;
}

std::unordered_map<std::string, wire::ptr> wire2map(const std::vector<wire::ptr>& wires)
{
    auto out = std::unordered_map<std::string, wire::ptr>();
    for (const auto& wire: wires)
        out[wire->name()] = wire;
    return out;
}

scope::scope(const std::vector<port::ptr>& ports)
: _parent(nullptr),
  _vars(port2wire(ports)),
  _name2var(wire2map(_vars))
{
}

scope::scope(const ptr& parent)
: _parent(parent),
  _vars(),
  _name2var()
{
}

scope::scope(const ptr& parent, const std::vector<wire::ptr>& wires)
: _parent(parent),
  _vars(wires),
  _name2var(wire2map(_vars))
{}

void scope::add_var(const wire::ptr& wire)
{
    _name2var[wire->name()] = wire;
    _vars.push_back(wire);
}

wire::ptr scope::lookup(const std::string& name) const
{
    auto l = _name2var.find(name);
    if (l == _name2var.end()) {
        if (_parent != nullptr)
            return _parent->lookup(name);

        return nullptr;
    }
    return l->second;
}
