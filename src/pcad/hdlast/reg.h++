// See LICENSE for details

#ifndef PCAD__HDLAST__REG_HXX
#define PCAD__HDLAST__REG_HXX

#include "wire.h++"

namespace pcad {
    namespace hdlast {
        /* "wire" and "reg" in Verilog are super similar. */
        class reg: public wire {
        public:
            typedef std::shared_ptr<reg> ptr;

        public:
            reg(const std::string& name, const long& width)
            : wire(name, width)
            {}

            reg(const std::string& name, const long& width, const long& depth)
            : wire(name, width, depth)
            {}
        };
    }
}

#endif
