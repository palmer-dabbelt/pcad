// See LICENSE for details

#ifndef PCAD__RTLIR__STATEMENT_HXX
#define PCAD__RTLIR__STATEMENT_HXX

#include "literal.h++"
#include "port.h++"
#include "wire.h++"
#include <memory>
#include <pcad/util/assert.h++>
#include <pcad/util/collection.h++>

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
            {
                util::assert(_wire != nullptr);
            }

        public:
            const decltype(_wire)& wire(void) const { return _wire; }
        };

        class port_statement: public wire_statement {
        public:
            typedef std::shared_ptr<port_statement> ptr;

        private:
            const rtlir::port::ptr _port;

        public:
            port_statement(const decltype(_port)& port)
            : wire_statement(port),
              _port(port)
            {}

        public:
            const decltype(_port)& port(void) const { return _port; }
            const auto& direction(void) const { return _port->direction(); }
        };

        /* A statement that represents a literal, which is a direct constant
         * (ie, a number). */
        class literal_statement: public statement {
        public:
            typedef std::shared_ptr<literal_statement> ptr;

        private:
            const rtlir::literal::ptr _data;

        public:
            literal_statement(const int& data)
            : _data( std::make_shared<literal>(data) )
            {
                util::assert(_data != nullptr);
            }

        public:
            const decltype(_data)& data(void) const { return _data; }
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
            {
                util::assert(_target != nullptr);
                util::assert(_source != nullptr);
            }

            connect_statement(
                const wire::ptr& target,
                const decltype(_source)& source
            ): _target( std::make_shared<wire_statement>(target) ),
               _source( source )
            {
                util::assert(_target != nullptr);
                util::assert(_source != nullptr);
            }

            connect_statement(
                const wire::ptr& target,
                const wire::ptr& source
            ): _target( std::make_shared<wire_statement>(target) ),
               _source( std::make_shared<wire_statement>(source) )
            {
                util::assert(_target != nullptr);
                util::assert(_source != nullptr);
            }

        public:
            const decltype(_target)& target(void) const { return _target; }
            const decltype(_source)& source(void) const { return _source; }
        };

        /* Connects a statement to a port. */
        class port_connect_statement: public statement {
        public:
            typedef std::shared_ptr<port_connect_statement> ptr;

        private:
            const port_statement::ptr _target;
            const statement::ptr _source;

        public:
            port_connect_statement(
                const decltype(_target)& target,
                const decltype(_source)& source
            ): _target(target),
               _source(source)
            {
                util::assert(_target != nullptr);
                util::assert(_source != nullptr);
            }

            port_connect_statement(
                const port::ptr& target,
                const decltype(_source)& source
            ): _target( std::make_shared<port_statement>(target) ),
               _source( source )
            {
                util::assert(_target != nullptr);
                util::assert(_source != nullptr);
            }

            port_connect_statement(
                const port::ptr& target,
                const wire::ptr& source
            ): _target( std::make_shared<port_statement>(target) ),
               _source( std::make_shared<wire_statement>(source) )
            {
                util::assert(_target != nullptr);
                util::assert(_source != nullptr);
            }

        public:
            const decltype(_target)& target(void) const { return _target; }
            const decltype(_source)& source(void) const { return _source; }
        };

        /* Extracts a bit field from a wire. */
        class slice_statement: public statement {
        public:
            typedef std::shared_ptr<slice_statement> ptr;

        private:
            const statement::ptr _source;
            const statement::ptr _hi;
            const statement::ptr _lo;

        public:
            slice_statement(
                const wire::ptr& source,
                const int& hi,
                const int& lo)
             : _source(std::make_shared<wire_statement>(source)),
               _hi(std::make_shared<literal_statement>(hi)),
               _lo(std::make_shared<literal_statement>(lo))
            {
                util::assert(_source != nullptr);
                util::assert(_hi != nullptr);
                util::assert(_lo != nullptr);
            }

        public:
            const decltype(_source)& source(void) const { return _source; }
            const decltype(_hi)& hi(void) const { return _hi; }
            const decltype(_lo)& lo(void) const { return _lo; }
        };

        /* Concatonates multiple wires together. */
        class cat_statement: public statement {
        public:
            typedef std::shared_ptr<cat_statement> ptr;

        private:
            const std::vector<statement::ptr> _sources;

        public:
            cat_statement(const std::vector<wire::ptr> sources)
            : _sources(
                putil::collection::map(
                    sources,
                    [](const auto& s) -> statement::ptr {
                        return std::make_shared<wire_statement>(s);
                    }
                ))
            {
                for (const auto& s: _sources)
                    util::assert(s != nullptr);
            }

            cat_statement(const wire::ptr& l, const wire::ptr& r)
            : _sources{
                std::make_shared<wire_statement>(l),
                std::make_shared<wire_statement>(r)
              }
            {
                for (const auto& s: _sources)
                    util::assert(s != nullptr);
            }

            cat_statement(const statement::ptr& l, const statement::ptr& r)
            : _sources{l, r}
            {
                for (const auto& s: _sources)
                    util::assert(s != nullptr);
            }

        public:
            const decltype(_sources)& sources(void) const { return _sources; }
        };

        /* Logical operators. */
        enum class binary_op {
            OR,
            AND,
        };

        class binop_statement: public statement {
        public:
            typedef std::shared_ptr<binop_statement> ptr;

        private:
            const binary_op _op;
            const statement::ptr _l;
            const statement::ptr _r;

        public:
            binop_statement(const decltype(_op)& op, const decltype(_l)& l, const decltype(_r)& r)
            : _op(op),
              _l(l),
              _r(r)
            {}

        public:
            const decltype(_op)& op(void) const { return _op; }
            const decltype(_l)& left(void) const { return _l; }
            const decltype(_r)& right(void) const { return _r; }
        };

        template <enum binary_op OP>
        class binop_statement_t: public binop_statement {
        public:
            binop_statement_t(const statement::ptr& l, const statement::ptr& r)
            : binop_statement(OP, l, r)
            {}

            binop_statement_t(const statement::ptr& l, const wire::ptr& r)
            : binop_statement(OP, l, std::make_shared<wire_statement>(r))
            {}
        };

        using or_statement = binop_statement_t<binary_op::OR>;
        using and_statement = binop_statement_t<binary_op::AND>;
    }
}

#endif
