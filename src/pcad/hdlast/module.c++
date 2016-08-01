// See LICENSE for details

#include "module.h++"
#include <iostream>
using namespace pcad::hdlast;

#if 0
/* http://stackoverflow.com/questions/874134/find-if-string-ends-with-another-string-in-c */
static inline
bool hasEnding (std::string const &fullString, std::string const &ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}

static inline
bool hasBeginning (std::string const &fullString, std::string const &beginning) {
    if (fullString.length() >= beginning.length()) {
        return (0 == fullString.compare (0, beginning.length(), beginning));
    } else {
        return false;
    }
}
#endif
