#pragma once
#include <string>
#include <vector>
#include <initializer_list>

namespace Angel::Logic::Parser
{
    struct Expression
    {
        Expression() = default;
        virtual ~Expression() = default;
    };

    struct Nothing : Expression
    {
    };

    struct Literal : Expression
    {
        Literal(const std::string_view literal) :
            literal(literal)
        {
        }
        std::string literal;
    };

    struct RegularExpression : Expression
    {
        RegularExpression(const std::string_view expression) :
            expression(expression)
        {
        }
        std::string expression;
    };

    struct Symbol : Expression
    {
        Symbol(const std::string_view label) :
            label(label)
        {
        }
        std::string label;
    };

    struct Disjunction : Expression
    {
        Disjunction(std::initializer_list<Expression> init) :
            expressions(init)
        {
        }
        std::vector<Expression> expressions;
    };

    struct Sequence : Expression
    {
        Sequence(std::initializer_list<Expression> init) : 
            expressions(init) 
        {
        }
        std::vector<Expression> expressions;
    };

    struct Loop : Expression
    {
        Expression Expression;
    };

    struct Whitespace : Expression
    {
    };

    struct Rule
    {
        Expression Expression;
    };

    struct RuleRef : Expression
    {
        RuleRef(Rule& rule) :
            rule(rule)
        {
        }
        Rule& rule;
    };
}