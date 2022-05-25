#pragma once
#include <string>
#include <vector>
#include <initializer_list>
#include <optional>
#include "Errors.h"

namespace Angel::Parser::BNF
{

    struct Match
    {
        const std::string_view remaining;
    };

    using PossibleMatch = std::optional<Match>;

    struct Expression
    {
        Expression() = default;
        virtual ~Expression() = default;
        virtual PossibleMatch Parse(const std::string_view data) const = 0;
    };

    struct Nothing : Expression
    {
        PossibleMatch Parse(const std::string_view data) const override;
    };

    struct Literal : Expression
    {
        Literal(const std::string_view literal) :
            literal(literal)
        {
        }
        std::string literal;
        PossibleMatch Parse(const std::string_view data) const override;
    };

    struct RegularExpression : Expression
    {
        RegularExpression(const std::string_view expression) :
            expression(expression)
        {
        }
        std::string expression;
        PossibleMatch Parse(const std::string_view data) const override;
    };

    struct Disjunction : Expression
    {
        Disjunction(std::initializer_list<const Expression> init)
        {
            expressions.reserve(init.size());
            for (auto& e : init)
            {
                expressions.emplace_back(&e);
            }
        }
        std::vector<const Expression*> expressions;
        PossibleMatch Parse(const std::string_view data) const override;
    };

    struct Sequence : Expression
    {
        Sequence(std::initializer_list<const Expression> init)
        {
            expressions.reserve(init.size());
            for (auto& e : init)
            {
                expressions.emplace_back(&e);
            }
        }
        std::vector<const Expression*> expressions;
        PossibleMatch Parse(const std::string_view data) const override;
    };

    struct Loop : Expression
    {
        const Expression& expresion;
        PossibleMatch Parse(const std::string_view data) const override;
    };

    struct Whitespace : Expression
    {
        PossibleMatch Parse(const std::string_view data) const override;
    };

    struct Rule 
    {
        Rule(const Expression& e)
            : expression(e)
        {
        }
        const Expression& expression;
        PossibleMatch Parse(const std::string_view data) const;
    };

    struct RuleRef : Expression
    {
        RuleRef(Rule& rule) :
            rule(rule)
        {
        }
        const Rule& rule;
        PossibleMatch Parse(const std::string_view data) const;
    };

    void Parse(const Rule& root, const std::string_view data);
}