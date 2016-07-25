// See LICENSE for details

#ifndef PCAD__RANDOM_HXX
#define PCAD__RANDOM_HXX

#include "wire.h++"

namespace pcad {
    /* A single random bit */
    class random: public wire {
    public:
        random(void)
        : wire("$random", 1)
        {}
    };
}

#endif
