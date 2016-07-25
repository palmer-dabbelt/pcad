// See LICENSE for details

#ifndef PCAD__STATEMENT_HXX
#define PCAD__STATEMENT_HXX

#include "scope.h++"
#include "literal.h++"
#include <memory>

namespace pcad {
    /* A single statement.  Statements exist within a scope. */
    class statement {
    public:
        typedef std::shared_ptr<statement> ptr;
    };

    /* There's a few differente types of statements. */
    class assign_statement: public statement {
    private:
        const wire::ptr _dest;
        const statement::ptr _source;
        
    public:
        assign_statement(const wire::ptr& dest,
                         const statement::ptr& source)
        : _dest(dest),
          _source(source)
        {}
    };

    class wire_statement: public statement {
    private:
        const wire::ptr _wire;

    public:
        wire_statement(const wire::ptr& wire)
        : _wire(wire)
        {}
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
    };
}

#endif
