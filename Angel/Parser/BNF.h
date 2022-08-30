#pragma once
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <functional>
#include <initializer_list>
#include "Errors.h"
#include "Match.h"

namespace Angel::Parser::BNF
{
    using Progress = std::map<const struct Rule*, size_t>;

    struct Expression
    {
        using MergeFn = std::function<std::any(const std::any& a, const std::any& b)>;

        Expression() = default;
        virtual ~Expression() = default;
        virtual std::unique_ptr<Expression> Copy() const = 0;
        virtual PossibleMatch Parse(const std::string_view data, const Progress& progress=Progress()) const = 0;
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
        PossibleMatch Parse(const std::string_view data, const Progress& progress = Progress()) const override;
        std::unique_ptr<Expression> Copy() const override { return std::make_unique<Nothing>(*this); }
    };

    struct Literal : Expression
    {
        Literal(const std::string_view literal) :
            literal(literal)
        {
        }
        std::string literal;
        PossibleMatch Parse(const std::string_view data, const Progress& progress = Progress()) const override;
        std::unique_ptr<Expression> Copy() const override { return std::make_unique<Literal>(*this); }
    };

    struct RegularExpression : Expression
    {
        RegularExpression(const std::string_view expression) :
            expression(expression)
        {
        }
        std::string expression;
        PossibleMatch Parse(const std::string_view data, const Progress& progress = Progress()) const override;
        std::unique_ptr<Expression> Copy() const override { return std::make_unique<RegularExpression>(*this); }
    };

    struct Whitespace : Expression
    {
        Whitespace(size_t amount=1, const std::string_view characters= " \t\r\n");
        PossibleMatch Parse(const std::string_view data, const Progress& progress = Progress()) const override;
        std::unique_ptr<Expression> Copy() const override { return std::make_unique<Whitespace>(*this); }
        size_t amount;
        std::string characters;
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
        PossibleMatch Parse(const std::string_view data, const Progress& progress = Progress()) const override;
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
        PossibleMatch Parse(const std::string_view data, const Progress& progress = Progress()) const override;
        std::unique_ptr<Expression> Copy() const override { return std::make_unique<Sequence>(*this); }
    };

    struct Loop : Expression
    {
        Loop(const Expression& e, MergeFn m)
            : expression(e)
            , merge(m)
        {
        }
        ExpressionRef expression;
        MergeFn merge;
        PossibleMatch Parse(const std::string_view data, const Progress& progress = Progress()) const override;
        std::unique_ptr<Expression> Copy() const override { return std::make_unique<Loop>(*this); }
    };

    struct Rule : Expression
    {
        using ParseFn = std::function<std::any(const std::string_view)>;
        using ConstructFn = std::function<std::any(std::any tokens)>;

        Rule(const std::string_view n, const Expression& e);
        Rule(const std::string_view n, const Expression& e, ParseFn p);
        Rule(const std::string_view n, const Expression& e, ConstructFn c);
        const std::string_view name;
        ExpressionRef expression;
        PossibleMatch Parse(const std::string_view data, const Progress& progress = Progress()) const;
        std::unique_ptr<Expression> Copy() const override { return std::make_unique<Rule>(*this); }
    protected:
        static std::any VoidParse(const std::string_view);
        static std::any PassToken(std::any tokens);

        std::any Tokenize(std::any tokens, const std::string_view data) const;

        bool recursive = false;
        ParseFn parse;
        ConstructFn construct;
    };

    struct Recursive : Rule
    {
        Recursive(const std::string_view n, const Expression& e, ConstructFn c=ConstructFn());
    };

    struct Declare : public Expression
    {
        Declare(const std::string_view rule);
        static void Define(const Rule& rule);
        const Rule& Get() const;
        PossibleMatch Parse(const std::string_view data, const Progress& progress = Progress()) const override;
        std::unique_ptr<Expression> Copy() const override { return std::make_unique<Declare>(*this); }
    private:
        const std::string_view rule;
        static std::map<const std::string_view, const Rule*> definitions;
    };


    struct Ref : Expression
    {
        Ref(Rule& rule) :
            rule(rule)
        {
        }
        Ref(Declare& declaration) :
            rule(declaration)
        {
        }
        ExpressionRef rule;
        PossibleMatch Parse(const std::string_view data, const Progress& progress = Progress()) const;
        std::unique_ptr<Expression> Copy() const override { return std::make_unique<Ref>(*this); }
    };

    Match Parse(const Rule& root , const std::string_view data);
}