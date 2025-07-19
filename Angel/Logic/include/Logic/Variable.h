#pragma once
#include "Logic/Hypothesis.h"
#include "Logic/Internal/Individual.h"
#include "Logic/Unary.h"
#include <string>

namespace Angel::Logic
{
class Expression;

// A variable is a sub category of Expression that can be assigned a value 
// on the first match as a substitution and subsequently provide that value 
// when matched again
class Variable  : public Individual
{
public:
    explicit Variable(const std::string_view name);
    using Individual::Individual;
    bool operator==(const Variable& var) const;
    std::size_t Hash() const;
    Variable Simplify() const;
    Set Assumptions() const;
    Expression Matches(const Expression& expression, const Hypothesis& hypothesis, bool reverse=false) const;
    Expression Substitute(const Hypothesis& hypothesis) const;
    Expression Infer(const class Knowledge& k, const Hypothesis& hypothesis, Trace& trace) const;
    constexpr static PrefixOperator ope{L'$'};
    using Pariant = class Operation;
};


std::ostream& operator<<(std::ostream& os, const Variable& id);

}
