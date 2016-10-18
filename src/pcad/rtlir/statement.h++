// See LICENSE for details

#ifndef PCAD__RTLIR__STATEMENT_HXX
#define PCAD__RTLIR__STATEMENT_HXX

#include "wire.h++"
#include <memory>

namespace pcad {
    namespace rtlir {
        class statement {
        public:
            typedef std::shared_ptr<statement> ptr;

        public:
            /* FIXME: This just lives here to make a vtable so I can do dynamic
             * casts.  It'll go away later. */
            virtual void nop(void) {}
        };

        /* Represents the value of a single named wire. */
        class wire_statement: public statement {
        public:
            typedef std::shared_ptr<wire_statement> ptr;

        private:
            const rtlir::wire::ptr _wire;

        public:
            wire_statement(const decltype(_wire)& wire)
            : _wire(wire)
            {}

        public:
            const decltype(_wire)& wire(void) const { return _wire; }
        };

        /* Connects two wires together, possibly unnamed ones. */
        class connect_statement: public statement {
        public:
            typedef std::shared_ptr<connect_statement> ptr;

        private:
            const statement::ptr _target;
            const statement::ptr _source;

        public:
            connect_statement(
                const decltype(_target)& target,
                const decltype(_source)& source
            ): _target(target),
               _source(source)
            {}

            connect_statement(
                const wire::ptr& target,
                const wire::ptr& source
            ): _target( std::make_shared<wire_statement>(target) ),
               _source( std::make_shared<wire_statement>(source) )
            {}

        public:
            const decltype(_target)& target(void) const { return _target; }
            const decltype(_source)& source(void) const { return _source; }
        };
    }
}

#endif
