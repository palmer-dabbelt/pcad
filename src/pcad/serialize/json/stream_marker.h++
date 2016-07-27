// See LICENSE for details

#ifndef PCAD__SERIALIZE__JSON__STREAM_MARKER_HXX
#define PCAD__SERIALIZE__JSON__STREAM_MARKER_HXX

namespace pcad {
    namespace serialize {
        namespace json {
            enum class stream_marker {
                BEGIN_STRUCTURE,
                END_STRUCTURE,
                BEGIN_ARRAY,
                END_ARRAY,
                NEXT_STRUCTURE,
            };
        }
    }
}

#endif
