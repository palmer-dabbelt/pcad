// See LICENSE for details

#include "json_stream.h++"
#include <iostream>
using namespace libjson;

ofstream::ofstream(const std::string& filename)
: _os(filename),
  _state_stack()
{
}

ofstream::~ofstream(void)
{
    if (_state_stack.size() > 0) {
        std::cerr << "There are still " << std::to_string(_state_stack.size()) << " JSON elements on the stack\n";
        abort();
    }

    _os << "\n";
    _os.close();
}

enum ofstream::stream_state ofstream::state(void) const
{
    if (_state_stack.size() == 0)
        return stream_state::NONE;

    return _state_stack.top();
}

std::string ofstream::indent(void) const
{
    std::string out = "";
    for (size_t i = 0; i < _state_stack.size(); ++i)
        out = out + "  ";
    return out;
}

ofstream* ofstream::operator<<(enum stream_marker marker)
{
    switch (marker) {
    case stream_marker::BEGIN_STRUCTURE:
        switch (state()) {
        case stream_state::ARRAY:
            _os << ",";
        case stream_state::ARRAY_START:
            _os << "\n";
            _state_stack.pop();
            _state_stack.push(stream_state::ARRAY);
        case stream_state::NONE:
            _os << indent() << "{";
            _state_stack.push(stream_state::STRUCTURE_START);
            break;

        case stream_state::STRUCTURE_START:
        case stream_state::STRUCTURE:
            std::cerr << "<<(BEGIN_STRUCTURE) can only start a JSON file or array\n";
            std::cerr << "  state was " << std::to_string((int)state());
            _os.close();
            abort();
            break;
        }
        break;

    case stream_marker::END_STRUCTURE:
        switch (state()) {
        case stream_state::STRUCTURE:
        case stream_state::STRUCTURE_START:
            _state_stack.pop();
            _os << "\n" << indent() << "}";
            break;

        case stream_state::NONE:
        case stream_state::ARRAY_START:
        case stream_state::ARRAY:
            std::cerr << "<<(END_STRUCTURE) must be paired with <<(START_STRUCTURE)\n";
            std::cerr << "  state was " << std::to_string((int)state());
            _os.close();
            abort();
            break;
        }
        break;

    case stream_marker::BEGIN_ARRAY:
        switch (state()) {
        case stream_state::NONE:
            _state_stack.push(stream_state::ARRAY_START);
            _os << "[";
            break;

        case stream_state::STRUCTURE_START:
        case stream_state::STRUCTURE:
        case stream_state::ARRAY_START:
        case stream_state::ARRAY:
            std::cerr << "<<(BEGIN_ARRAY) can only start a JSON file\n";
            _os.close();
            abort();
            break;
        }
        break;

    case stream_marker::END_ARRAY:
        switch (state()) {
        case stream_state::ARRAY:
        case stream_state::ARRAY_START:
            _state_stack.pop();
            _os << "\n" << indent() << "]";
            break;

        case stream_state::NONE:
        case stream_state::STRUCTURE_START:
        case stream_state::STRUCTURE:
            std::cerr << "<<(END_ARRAY) must be paired with <<(START_ARRAY)\n";
            std::cerr << "  state was " << std::to_string((int)state());
            _os.close();
            abort();
            break;
        }
        break;
    }

    return this;
}

ofstream* ofstream::operator<<(const std::pair<std::string, std::string>& kv)
{
    switch (state()) {
    case stream_state::STRUCTURE:
        _os << ",";
    case stream_state::STRUCTURE_START:
        _os << "\n" << indent() << "\"" << kv.first << "\": \"" << kv.second << "\"";
        _state_stack.pop();
        _state_stack.push(stream_state::STRUCTURE);
        break;

    case stream_state::NONE:
    case stream_state::ARRAY_START:
    case stream_state::ARRAY:
        _os.close();
        abort();
        break;
    }
    return this;
}

ofstream* ofstream::operator<<(const std::pair<std::string, enum stream_marker>& kv)
{
    auto key = kv.first;
    auto marker = kv.second;

    switch (marker) {
    case stream_marker::BEGIN_STRUCTURE:
        switch (state()) {
        case stream_state::NONE:
        case stream_state::STRUCTURE_START:
        case stream_state::STRUCTURE:
        case stream_state::ARRAY_START:
        case stream_state::ARRAY:
            _os.close();
            abort();
            break;
        }
        break;

    case stream_marker::BEGIN_ARRAY:
        switch (state()) {
        case stream_state::STRUCTURE:
            _os << ",";
        case stream_state::STRUCTURE_START:
            _os << "\n" << indent() << "\"" << key << "\": [";
            _state_stack.push(stream_state::ARRAY_START);
            break;

        case stream_state::NONE:
        case stream_state::ARRAY_START:
        case stream_state::ARRAY:
            std::cerr << "<<(string, BEGIN_ARRAY) needs to be within a structure\n";
            std::cerr << "  state was " << std::to_string((int)state());
            abort();
            _os.close();
            break;
        }
        break;

    case stream_marker::END_STRUCTURE:
    case stream_marker::END_ARRAY:
        _os.close();
        abort();
        break;
    }

    return this;
}
