#pragma once 
#include "Logic/Match.h"
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
    Match Matches(const Expression& expression, const Variables& vars) const;
    Expression Infer(const class Knowledge& k, const Variables& substitutions) const;
    bool operator==(const Negative& other) const;
    constexpr static UnaryOperator ope{L'-'};
};

std::ostream& operator<<(std::ostream& os, const Negative& );

}