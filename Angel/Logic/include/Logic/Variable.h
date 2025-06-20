#pragma once
#include "Logic/Hypothesis.h"
#include <string>

namespace Angel::Logic
{
class Expression;

// A variable is a sub category of Expression that can be assigned a value 
// on the first match as a substitution and subsequently provide that value 
// when matched again
class Variable 
{
public:
    explicit Variable(const std::string_view name);
    explicit operator bool() const;
    bool operator==(const Variable& id) const;
    std::size_t Hash() const;
    std::string_view Name() const;
    Variable Simplify() const;
    Expression Matches(const Expression& expression, const Hypothesis& hypothesis, bool reverse=false) const;
    Expression Substitute(const Hypothesis& hypothesis) const;
    Expression Infer(const class Knowledge& k, const Hypothesis& hypothesis, Trace& trace) const;
private: 
    std::string name;
};


std::ostream& operator<<(std::ostream& os, const Variable& id);

}
