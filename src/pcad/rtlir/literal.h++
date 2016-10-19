// See LICENSE for details

#ifndef PCAD__RTLIR__LITERAL_HXX
#define PCAD__RTLIR__LITERAL_HXX

#include <memory>
#include <string>

namespace pcad {
    namespace rtlir {
        class literal {
        public:
            typedef std::shared_ptr<literal> ptr;

        private:
            const int _data;

        public:
            literal(int data)
            : _data(data)
            {}

        public:
            std::string as_string(void) const { return std::to_string(_data); }
        };
    }
}

#endif
