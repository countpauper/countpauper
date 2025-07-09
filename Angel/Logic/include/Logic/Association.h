#pragma once
#include "Logic/Hypothesis.h"
#include "Logic/Binary.h"
#include "Logic/Internal/Pair.h"
#include <memory>

namespace Angel::Logic
{

class Expression;

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
    Expression Infer(const class Knowledge& k, const Hypothesis& hypothesis, Trace& trace) const;
    Expression Get(const Expression& key) const;
    bool operator==(const Association& other) const { return Pair::operator==(other); }
    bool IsClause() const;
    constexpr static BinaryOperator ope{L'←'};
};


std::ostream& operator<<(std::ostream& os, const Association& a);

}