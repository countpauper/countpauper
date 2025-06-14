#pragma once
#include "Logic/Hypothesis.h"
#include <string>
#include <vector>
#include <ranges>

namespace Angel::Logic
{
class Expression;
using Collection_subrange = std::ranges::subrange<std::vector<Expression>::const_iterator, std::vector<Expression>::const_iterator>;


// A Tuple is a sub category of Expression that can be match with a sequence of values
class Tuple
{
public:
    explicit Tuple(const std::string_view name);
    explicit operator bool() const;
    bool operator==(const Tuple& id) const;
    std::size_t Hash() const;
    std::string_view Name() const;
    Tuple Simplify() const;
    Expression Matches(Collection_subrange range, const Hypothesis& substituions, bool reverse=false) const;
    Expression Matches(const Expression& expression, const Hypothesis& hypothesis, bool reverse=false) const;
    Expression Substitute(const Hypothesis& hypothesis) const;
    Expression Infer(const class Knowledge& k, const Hypothesis& hypothesis, Trace& trace) const;
private: 
    std::string name;
};


std::ostream& operator<<(std::ostream& os, const Tuple& id);

}
