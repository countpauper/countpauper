#pragma once 
#include "Logic/Hypothesis.h"
#include "Logic/Individual.h"
#include "Logic/Unary.h"
#include <memory>

namespace Angel::Logic
{
class Expression;
class Negative : public Individual
{
public: 
    using Individual::Individual;

    Expression Simplify() const;
    Expression Matches(const Expression& expression, const Hypothesis& substutions) const;
    Negative Substitute(const Hypothesis& hypothesis) const;
    Expression Infer(const class Knowledge& k, const Hypothesis& hypothesis, Trace& trace) const;
    bool operator==(const Negative& other) const;
    constexpr static PrefixOperator ope{L'-'};
};

std::ostream& operator<<(std::ostream& os, const Negative& );

}