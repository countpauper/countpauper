#pragma once 
#include "Logic/Hypothesis.h"
#include "Logic/Individual.h"
#include "Logic/Operator.h"
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
    Expression Infer(const class Knowledge& k, Hypothesis& hypothesis) const;
    bool operator==(const Negative& other) const;
    constexpr static UnaryOperator ope{L'-'};
};

std::ostream& operator<<(std::ostream& os, const Negative& );

}