// See LICENSE for details

#ifndef LIBJSON__STREAM_MARKER_HXX
#define LIBJSON__STREAM_MARKER_HXX

namespace libjson {
    enum class stream_marker {
        BEGIN_STRUCTURE,
        END_STRUCTURE,
        BEGIN_ARRAY,
        END_ARRAY,
        NEXT_STRUCTURE,
    };
}

#endif
