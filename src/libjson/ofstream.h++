// See LICENSE for details

#ifndef LIBJSON__OFSTREAM_HXX
#define LIBJSON__OFSTREAM_HXX

#include "stream_marker.h++"
#include <fstream>
#include <map>
#include <stack>

namespace libjson {
    /* Allows users to stream out C++ objects to JSON. */
    class ofstream {
    private:
        enum class stream_state {
            NONE,
            STRUCTURE_START,
            STRUCTURE,
            ARRAY_START,
            ARRAY,
            STRUCTURE_NEXT,
        };

    private:
        std::ofstream _os;
        std::stack<enum stream_state> _state_stack;

    public:
        ofstream(const std::string& filename);

        virtual ~ofstream(void);

    private:
        enum stream_state state(void) const;
        std::string indent(void) const;

    public:
        libjson::ofstream* operator<<(enum stream_marker marker);
        libjson::ofstream* operator<<(const std::pair<std::string, std::string>& kv);
        libjson::ofstream* operator<<(const std::pair<std::string, enum stream_marker>& kv);
    };

    /* This is just a utility function: frequently I want to set a key/value
     * pair but I want to write a "const char *". */
    static inline std::pair<std::string, std::string>
    make_pair(const std::string& a, const std::string& b)
    { return std::make_pair(a, b); }

    static inline std::pair<std::string, enum stream_marker>
    make_pair(const std::string& a, enum stream_marker b)
    { return std::make_pair(a, b); }
}

#endif
