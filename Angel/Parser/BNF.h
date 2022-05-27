#pragma once
#include <string>
#include <vector>
#include <memory>
#include <initializer_list>
#include "Errors.h"
#include "Match.h"

namespace Angel::Parser::BNF
{
    class Interpreter;

    struct Expression
    {
        Expression() = default;
        virtual ~Expression() = default;
        virtual std::unique_ptr<Expression> Copy() const = 0;
        virtual PossibleMatch Parse(const std::string_view data, const Interpreter& interpreter) const = 0;
    };

    class ExpressionRef
    {
    public:
        ExpressionRef(const Expression& original) :
            ref(original.Copy())
        {
        }
        ExpressionRef(const ExpressionRef& other) :
            ref(other.ref->Copy())
        {
        }
        ExpressionRef(ExpressionRef&& other) :
            ref(std::move(other.ref))
        {
        }
        ExpressionRef& operator=(const ExpressionRef& other)
        {
            ref = other.ref->Copy();
            return *this;
        }
        ExpressionRef& operator=(const Expression& other)
        {
            ref = other.Copy();
            return *this;
        }
        const Expression& operator*() const { return *ref; }
        const Expression* operator->() const { return ref.get();  }
    private:
        std::unique_ptr<Expression> ref;
    };

    struct Nothing : Expression
    {
        PossibleMatch Parse(const std::string_view data, const Interpreter& interpreter) const override;
        std::unique_ptr<Expression> Copy() const override { return std::make_unique<Nothing>(*this); }
    };

    struct Literal : Expression
    {
        Literal(const std::string_view literal) :
            literal(literal)
        {
        }
        std::string literal;
        PossibleMatch Parse(const std::string_view data, const Interpreter& interpreter) const override;
        std::unique_ptr<Expression> Copy() const override { return std::make_unique<Literal>(*this); }
    };

    struct RegularExpression : Expression
    {
        RegularExpression(const std::string_view expression) :
            expression(expression)
        {
        }
        std::string expression;
        PossibleMatch Parse(const std::string_view data, const Interpreter& interpreter) const override;
        std::unique_ptr<Expression> Copy() const override { return std::make_unique<RegularExpression>(*this); }
    };

    struct Whitespace : Expression
    {
        PossibleMatch Parse(const std::string_view data, const Interpreter& interpreter) const override;
        std::unique_ptr<Expression> Copy() const override { return std::make_unique<Whitespace>(*this); }
    };

    struct Disjunction : Expression
    {
        Disjunction() = default;
        template<class ...Args>
        Disjunction(const Expression& first, const Args& ... args) :
            Disjunction(std::forward<const Args&>(args)...)
        {
            expressions.emplace(expressions.begin(), first);
        }
        ~Disjunction()
        {
        }
        std::vector<ExpressionRef> expressions;
        PossibleMatch Parse(const std::string_view data, const Interpreter& interpreter) const override;
        std::unique_ptr<Expression> Copy() const override { return std::make_unique<Disjunction>(*this); }
    };

    struct Sequence : Expression
    {
        Sequence() = default;
        template<class ...Args>
        Sequence(const Expression& first, const Args& ... args) :
            Sequence(std::forward<const Args&>(args)...)
        {
            expressions.emplace(expressions.begin(), first);
        }
        std::vector<ExpressionRef> expressions;
        PossibleMatch Parse(const std::string_view data, const Interpreter& interpreter) const override;
        std::unique_ptr<Expression> Copy() const override { return std::make_unique<Sequence>(*this); }
    };

    struct Loop : Expression
    {
        Loop(const Expression& e)
            : expression(e)
        {
        }
        ExpressionRef expression;
        PossibleMatch Parse(const std::string_view data, const Interpreter& interpreter) const override;
        std::unique_ptr<Expression> Copy() const override { return std::make_unique<Loop>(*this); }
    };

    struct Rule : Expression
    {
        Rule(const std::string_view n, const Expression& e)
            : name(n), expression(e)
        {
        }
        const std::string_view name;
        ExpressionRef expression;
        PossibleMatch Parse(const std::string_view data, const Interpreter& interpreter) const;
        std::unique_ptr<Expression> Copy() const override { return std::make_unique<Rule>(*this); }
    };

    struct Ref : Expression
    {
        Ref(Rule& rule) :
            rule(rule)
        {
        }
        ExpressionRef rule;
        PossibleMatch Parse(const std::string_view data, const Interpreter& interpreter) const;
        std::unique_ptr<Expression> Copy() const override { return std::make_unique<Ref>(*this); }
    };

    Match Parse(const Rule& root, const Interpreter& interpreter, const std::string_view data);
}