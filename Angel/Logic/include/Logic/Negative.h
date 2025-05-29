#pragma once 
#include "Logic/Substitution.h"
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
    Expression Matches(const Expression& expression, const Substitutions& substutions) const;
    Negative Substitute(const Substitutions& substitutions) const;
    Expression Infer(const class Knowledge& k, const Substitutions& substitutions) const;
    bool operator==(const Negative& other) const;
    constexpr static UnaryOperator ope{L'-'};
};

std::ostream& operator<<(std::ostream& os, const Negative& );

}