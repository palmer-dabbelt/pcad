// See LICENSE for details

#ifndef PCAD__HDLAST__STATEMENT_HXX
#define PCAD__HDLAST__STATEMENT_HXX

#include "scope.h++"
#include "literal.h++"
#include <memory>
#include <pcad/util/assert.h++>

namespace pcad {
    namespace hdlast {
        /* A single statement.  Statements exist within a scope. */
        class statement {
        public:
            typedef std::shared_ptr<statement> ptr;

            virtual ~statement(void) {}

        protected:
            statement(void) {}
        };

        /* There's a few differente types of statements. */
        class assign_statement: public statement {
        private:
            const statement::ptr _dest;
            const statement::ptr _source;
            
        public:
            assign_statement(const statement::ptr& dest,
                             const statement::ptr& source)
            : _dest(dest),
              _source(source)
            {
                util::assert(_dest != nullptr);
                util::assert(_source != nullptr);
            }

        public:
            const decltype(_dest)& dest(void) const { return _dest; }
            const decltype(_source)& source(void) const { return _source; }
        };

        class wire_statement: public statement {
        private:
            const hdlast::wire::ptr _wire;

        public:
            wire_statement(const hdlast::wire::ptr& wire)
            : _wire(wire)
            {
                util::assert(_wire != nullptr);
            }

        public:
            const decltype(_wire)& wire(void) const { return _wire; }
        };

        class unop_statement: public statement {
        public:
            enum class op {
                NOT,
                BANG,
            };

        private:
            enum op _op;
            const statement::ptr _statement;

        public:
            unop_statement(enum op operation,
                           const statement::ptr& statement)
            : _op(operation),
              _statement(statement)
            {
                util::assert(_statement != nullptr);
            }

            const decltype(_op)& opcode(void) const { return _op; }
            const decltype(_statement)& statement(void) const { return _statement; }
        };

        class biop_statement: public statement {
        public:
            enum class op {
                AND,
                OR,
                EQEQ,
                LT,
                SUB,
                LTEQ,
                ADD,
                NEQ,
                MUL,
                XOR,
                GTEQ,
                LSH,
                RSHL,
                RSHA,
                GT,
                OROR,
                DIV,
                ANDAND,
                EQEQEQ
            };

        private:
            enum biop_statement::op _op;
            const statement::ptr _a;
            const statement::ptr _b;

        public:
            biop_statement(const decltype(_op)& operation,
                           const statement::ptr& a,
                           const statement::ptr& b)
            : _op(operation),
              _a(a),
              _b(b)
            {
                util::assert(_a != nullptr);
                util::assert(_b != nullptr);
            }

        public:
            const decltype(_op)& opcode(void) const { return _op; }
            const decltype(_a)& a(void) const { return _a; }
            const decltype(_b)& b(void) const { return _b; }

        public:
            /* GCC seems to have trouble with to_string on my nested op enum,
             * so I'm just doing this for now. */
            std::string opstr(void) const { return opstr(_op); }

        public:
            static std::string opstr(const enum op& op);
        };

        class slice_statement: public statement {
        private:
            const statement::ptr _source;
            const statement::ptr _hi;
            const statement::ptr _lo;

        public:
            slice_statement(const statement::ptr& source,
                            const statement::ptr& hi,
                            const statement::ptr& lo)
            : _source(source),
              _hi(hi),
              _lo(lo)
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

        class index_statement: public statement {
        private:
            const statement::ptr _source;
            const statement::ptr _index;

        public:
            index_statement(const statement::ptr& source,
                            const statement::ptr index)
            : _source(source),
              _index(index)
            {
                util::assert(_source != nullptr);
                util::assert(_index != nullptr);
            }

        public:
            const decltype(_source) source(void) const { return _source; }
            const decltype(_index) index(void) const { return _index; }
        };

        class cat_statement: public statement {
        private:
            const statement::ptr _hi;
            const statement::ptr _lo;

        public:
            cat_statement(const statement::ptr& hi,
                          const statement::ptr& lo)
            : _hi(hi),
              _lo(lo)
            {}

        public:
            const decltype(_hi)& hi(void) const { return _hi; }
            const decltype(_lo)& lo(void) const { return _lo; }
        };

        class trop_statement: public statement {
        private:
            const statement::ptr _sel;
            const statement::ptr _ont;
            const statement::ptr _onf;

        public:
            trop_statement(const statement::ptr& sel,
                           const statement::ptr& ont,
                           const statement::ptr& onf)
            : _sel(sel),
              _ont(ont),
              _onf(onf)
            {
                util::assert(_sel != nullptr);
                util::assert(_ont != nullptr);
                util::assert(_onf != nullptr);
            }

        public:
            const decltype(_sel) sel(void) const { return _sel; }
            const decltype(_ont) ont(void) const { return _ont; }
            const decltype(_onf) onf(void) const { return _onf; }
        };

        class rep_statement: public statement {
        private:
            const literal::ptr _width;
            const statement::ptr _value;

        public:
            rep_statement(const literal::ptr& width,
                          const statement::ptr& value)
            : _width(width),
              _value(value)
            {
                util::assert(_width != nullptr);
                util::assert(_value != nullptr);
            }

        public:
            const decltype(_width)& width(void) const { return _width; }
            const decltype(_value)& value(void) const { return _value; }
        };

        class builtin_statement: public statement {
        private:
            const std::string _function;
            const statement::ptr _arg;

        public:
            builtin_statement(const std::string& function,
                              const statement::ptr& usv)
            : _function(function),
              _arg(usv)
            {
                util::assert(_arg != nullptr);
            }

        public:
            const decltype(_function) function(void) const { return _function; }
            const decltype(_arg) arg(void) const { return _arg; }
        };

        class always_statement: public statement {
        private:
            const statement::ptr _trigger;
            const std::vector<statement::ptr> _body;

        public:
            always_statement(const statement::ptr& trigger,
                             const std::vector<statement::ptr>& body)
            : _trigger(trigger),
              _body(body)
            {
                util::assert(_trigger != nullptr);
                for (const auto& e: _body)
                    util::assert(e != nullptr);
            }

        public:
            const decltype(_trigger)& trigger(void) const { return _trigger; }
            const decltype(_body)& body(void) const { return _body; }
        };

        class posedge_statement: public statement {
        private:
            const wire::ptr _clock;

        public:
            posedge_statement(const wire::ptr& clock)
            : _clock(clock)
            {
                util::assert(_clock != nullptr);
            }

        public:
            const decltype(_clock)& clock(void) const { return _clock; }
        };

        class if_statement: public statement {
        private:
            const statement::ptr _cond;
            const std::vector<statement::ptr> _on_true;
            const std::vector<statement::ptr> _on_false;

        public:
            if_statement(const statement::ptr& cond,
                         const std::vector<statement::ptr>& on_true,
                         const std::vector<statement::ptr>& on_false)
            : _cond(cond),
              _on_true(on_true),
              _on_false(on_false)
            {
                util::assert(_cond != nullptr);
                for (const auto& e: _on_true)
                    util::assert(e != nullptr);
                for (const auto& e: _on_false)
                    util::assert(e != nullptr);
            }

        public:
            const decltype(_cond)& cond(void) const { return _cond; }
            const decltype(_on_true)& on_true(void) const { return _on_true; }
            const decltype(_on_false)& on_false(void) const { return _on_false; }
        };
    }
}

#endif
