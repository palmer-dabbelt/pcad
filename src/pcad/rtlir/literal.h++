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
            const int _width;

        public:
            literal(int data)
            : _data(data),
              _width(32)
            {}

            literal(int data, int width)
            : _data(data),
              _width(width)
            {}

        public:
            std::string as_string(void) const { return std::to_string(_data); }
            auto data(void) const { return _data; }
            auto width(void) const { return _width; }
        };
    }
}

#endif
