// See LICENES for details

#ifndef PCAD__RTLIR__PORT_HXX
#define PCAD__RTLIR__PORT_HXX

#include <memory>

namespace pcad {
    namespace rtlir {
        class port {
        public:
            typedef std::shared_ptr<port> ptr;
        };
    }
}

#endif
