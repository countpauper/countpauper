#pragma once
#include <string>
#include <vector>
#include <memory>
#include <initializer_list>
#include <optional>
#include "Errors.h"

namespace Angel::Parser::BNF
{

    class Match
    {
    public:
        Match(const std::string_view r) :
            remaining(r)
        {
        }
        Match(const std::string_view name, const Match& child) :
            remaining(child.remaining)
        {
            for (auto& cr : child.results)
            {
                if (cr.first[0] == '[')
                    result(std::string(name)+cr.first,  cr.second);
                else
                    result(std::string(name) + "." + cr.first, cr.second);
            }
        }
        const std::string_view operator[](const std::string_view k) const
        {
            return results.at(k.data()).c_str();
        }
        void result(const std::string_view k, const std::string_view v)
        {
            results[k.data()] = v; // TODO: append if it already exists
        }
        const std::string_view remaining;
    private:
        // TODO: nested results, vector results, get out with key dots, indices
        std::map<std::string, std::string> results;
    };

    using PossibleMatch = std::optional<Match>;

    struct Expression
    {
        Expression() = default;
        virtual ~Expression() = default;
        virtual std::unique_ptr<Expression> Copy() const = 0;
        virtual PossibleMatch Parse(const std::string_view data) const = 0;
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
        PossibleMatch Parse(const std::string_view data) const override;
        std::unique_ptr<Expression> Copy() const override { return std::make_unique<Nothing>(*this); }
    };

    struct Literal : Expression
    {
        Literal(const std::string_view literal) :
            literal(literal)
        {
        }
        std::string literal;
        PossibleMatch Parse(const std::string_view data) const override;
        std::unique_ptr<Expression> Copy() const override { return std::make_unique<Literal>(*this); }
    };

    struct RegularExpression : Expression
    {
        RegularExpression(const std::string_view expression) :
            expression(expression)
        {
        }
        std::string expression;
        PossibleMatch Parse(const std::string_view data) const override;
        std::unique_ptr<Expression> Copy() const override { return std::make_unique<RegularExpression>(*this); }
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
        PossibleMatch Parse(const std::string_view data) const override;
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
        PossibleMatch Parse(const std::string_view data) const override;
        std::unique_ptr<Expression> Copy() const override { return std::make_unique<Sequence>(*this); }
    };

    struct Loop : Expression
    {
        Loop(const Expression& e)
            : expression(e)
        {
        }
        ExpressionRef expression;
        PossibleMatch Parse(const std::string_view data) const override;
        std::unique_ptr<Expression> Copy() const override { return std::make_unique<Loop>(*this); }
    };

    struct Whitespace : Expression
    {
        PossibleMatch Parse(const std::string_view data) const override;
        std::unique_ptr<Expression> Copy() const override { return std::make_unique<Whitespace>(*this); }
    };

    struct Rule : Expression
    {
        Rule(const std::string_view n, const Expression& e)
            : name(n), expression(e)
        {
        }
        const std::string_view name;
        ExpressionRef expression;
        PossibleMatch Parse(const std::string_view data) const;
        std::unique_ptr<Expression> Copy() const override { return std::make_unique<Rule>(*this); }
    };

    struct Ref : Expression
    {
        Ref(Rule& rule) :
            rule(rule)
        {
        }
        ExpressionRef rule;
        PossibleMatch Parse(const std::string_view data) const;
        std::unique_ptr<Expression> Copy() const override { return std::make_unique<Ref>(*this); }
    };

    Match Parse(const Rule& root, const std::string_view data);
}