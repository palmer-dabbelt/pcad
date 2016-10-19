// See LICENSE for details

#ifndef PCAD__UTIL__ASSERT_HXX
#define PCAD__UTIL__ASSERT_HXX

#ifndef NDEBUG
#include <iostream>
#endif

namespace pcad {
    namespace util {
        static inline void assert(int expression) {
#ifndef NDEBUG
            if (expression == 0) {
                abort();
            }
#endif
        }

        static inline void assert(int expression, const std::string& message) {
#ifndef NDEBUG
            if (expression == 0) {
                std::cerr << "assertion failed: " << message << "\n";
                abort();
            }
#endif
        }
    }
}

#endif
