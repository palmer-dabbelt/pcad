// See LICENSE for details

#ifndef PCAD__HDLAST__LITERAL_HXX
#define PCAD__HDLAST__LITERAL_HXX

#include "wire.h++"

namespace pcad {
    namespace hdlast {
        /* A literal value. */
        class literal: public wire {
        public:
            typedef std::shared_ptr<literal> ptr;

        public:
            literal(long value,
                    long width)
            : wire(std::to_string(value), width)
            {}
        };
    }
}

#endif
