// See LICENSE for details

#ifndef PCAD__HDLAST__BLACKBOX_HXX
#define PCAD__HDLAST__BLACKBOX_HXX

#include "module.h++"

namespace pcad {
    namespace hdlast {
        class blackbox: public module {
        public:
            typedef std::shared_ptr<blackbox> ptr;

        public:
            blackbox(const module::ptr& m)
            : module(
                m->name(),
                m->ports(),
                m->body(),
                m->statements(),
                m->instances()
            )
            {}
        };
    }
}

#endif
