// See LICENSE for details

#ifndef PCAD__STATEMENT_HXX
#define PCAD__STATEMENT_HXX

#include "scope.h++"
#include "literal.h++"
#include "serialize/json/ofstream.h++"
#include <memory>

namespace pcad {
    /* A single statement.  Statements exist within a scope. */
    class statement {
    public:
        typedef std::shared_ptr<statement> ptr;

        /* This one is funny -- it expects a BEGIN/END pair outside it! */
        virtual void dump(pcad::serialize::json::ofstream& os) = 0;
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
        {}

        virtual void dump(pcad::serialize::json::ofstream& os);
    };

    class wire_statement: public statement {
    private:
        const wire::ptr _wire;

    public:
        wire_statement(const wire::ptr& wire)
        : _wire(wire)
        {}

        virtual void dump(pcad::serialize::json::ofstream& os);
    };

    class unop_statement: public statement {
    public:
        enum class op {
            NOT
        };

    private:
        enum op _op;
        const statement::ptr _statement;

    public:
        unop_statement(enum op operation,
                       const statement::ptr& statement)
        : _op(operation),
          _statement(statement)
        {}

        virtual void dump(pcad::serialize::json::ofstream& os);
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
        };

    private:
        enum op _op;
        const statement::ptr _a;
        const statement::ptr _b;

    public:
        biop_statement(enum op operation,
                       const statement::ptr& a,
                       const statement::ptr& b)
        : _op(operation),
          _a(a),
          _b(b)
        {}

        virtual void dump(pcad::serialize::json::ofstream& os);
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
        {}

        virtual void dump(pcad::serialize::json::ofstream& os);
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

        virtual void dump(pcad::serialize::json::ofstream& os);
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
        {}

        virtual void dump(pcad::serialize::json::ofstream& os);
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
        {}

        virtual void dump(pcad::serialize::json::ofstream& os);
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
        {}

        virtual void dump(pcad::serialize::json::ofstream& os);
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
        {}

        virtual void dump(pcad::serialize::json::ofstream& os);
    };

    class posedge_statement: public statement {
    private:
        const wire::ptr _clock;

    public:
        posedge_statement(const wire::ptr& clock)
        : _clock(clock)
        {}

        virtual void dump(pcad::serialize::json::ofstream& os);
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
        {}

        virtual void dump(pcad::serialize::json::ofstream& os);
    };
}

#endif
