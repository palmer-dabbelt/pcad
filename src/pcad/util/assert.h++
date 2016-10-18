// See LICENSE for details

#ifndef NDEBUG
#include <iostream>
#endif

namespace pcad {
    namespace util {
        void assert(int expression, const std::string& message) {
#ifndef NDEBUG
            if (expression == 0) {
                std::cerr << "assertion failed: " << message << "\n";
                abort();
            }
#endif
        }
    }
}
