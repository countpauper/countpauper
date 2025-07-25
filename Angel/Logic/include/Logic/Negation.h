#pragma once 
#include "Logic/Hypothesis.h"
#include "Logic/Internal/Individual.h"
#include "Logic/Unary.h"
#include <memory>

namespace Angel::Logic
{
class Expression;
class Integer;

class Negation : public Individual
{
public: 
    using Individual::Individual;

    Expression Simplify() const;
    Expression Matches(const Expression& expression, const Hypothesis& substutions) const;
    Negation Substitute(const Hypothesis& hypothesis) const;
    Expression Infer(const class Knowledge& k, const Hypothesis& hypothesis, Trace& trace) const;
    bool operator==(const Negation& other) const;
    std::size_t Hash() const;
    constexpr static PrefixOperator ope{L'¬'};
    using Pariant = class Operation;
};

std::ostream& operator<<(std::ostream& os, const Negation& );

}