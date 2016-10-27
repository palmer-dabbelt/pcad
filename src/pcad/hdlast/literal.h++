// See LICENSE for details

#ifndef PCAD__HDLAST__LITERAL_HXX
#define PCAD__HDLAST__LITERAL_HXX

#include "wire.h++"
#include <sstream>

namespace pcad {
    namespace hdlast {
        /* A literal value. */
        class literal: public wire {
        public:
            typedef std::shared_ptr<literal> ptr;

        private:
            long _value;

        public:
            literal(long value,
                    long width)
            : wire(std::to_string(value), width),
              _value(value)
            {}

            literal(const std::string& value,
                    long width)
            : wire(value, width),
              _value(std::stoi(value))
            {}

        public:
            virtual std::string as_string(void) const {
                std::stringstream ss;
                ss << width() << "'" << "d" << _value;
                return ss.str();
            }

            auto value(void) const { return _value; }

        };
    }
}

#endif
