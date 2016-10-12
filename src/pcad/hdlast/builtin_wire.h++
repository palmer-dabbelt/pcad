// See LICENSE for details

#ifndef PCAD__HDLAST__RANDOM_HXX
#define PCAD__HDLAST__RANDOM_HXX

#include "wire.h++"

namespace pcad {
    namespace hdlast {
        /* A single random bit */
        class random: public wire {
        public:
            random(void)
            : wire("$random", 1)
            {}
        };

        /* All visible signals. */
        class star: public wire {
        public:
            star(void)
            : wire("*", 0)
            {}
        };

        /* A single-bit x */
        class x: public wire {
            public:
            x(void)
            : wire("1'bX", 1)
            {}
        };
    }
}

#endif
