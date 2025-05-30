#pragma once
#include "Logic/Hypothesis.h"
#include "Logic/Operator.h"
#include <memory>

namespace Angel::Logic
{

class Expression;

class Pair 
{   // TODO: a Range is another type of Pair
public:
    Pair(Expression&& lhs, Expression&& rhs);
    explicit operator bool() const;
    std::size_t size() const;
    const Expression& Left() const;
    Expression& Left();
    const Expression& Right() const;
    Expression& Right();
    std::size_t Hash() const;
protected:
    Pair(const Pair& o);
    Pair(Pair&& o); 
    Pair& operator=(const Pair& o);
    Pair& operator=(Pair&& o);
    bool operator==(const Pair& o) const;

    std::unique_ptr<Expression> lhs;
    std::unique_ptr<Expression> rhs;
};

class Association : public Pair
{
public:
    Association(const Association& o): Pair(o) {}
    Association(Association&& o) : Pair(std::move(o)) {}
    using Pair::Pair;

    Association& operator=(const Association& o);
    Association& operator=(Association&& o);

    Expression Simplify() const;
    Expression Matches(const Expression& expression, const Hypothesis& hypothesis) const;
    Association Substitute(const Hypothesis& hypothesis) const;
    Expression Infer(const class Knowledge& k, Hypothesis& hypothesis) const;
    Expression Get(const Expression& key) const;
    bool operator==(const Association& other) const { return Pair::operator==(other); }

    constexpr static BinaryOperator ope{L':'};
};


std::ostream& operator<<(std::ostream& os, const Association& a);

}