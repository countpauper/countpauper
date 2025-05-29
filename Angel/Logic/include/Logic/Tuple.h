#pragma once
#include "Logic/Match.h"
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
    Expression Simplify() const;
    Match Matches(Collection_subrange range, const Variables& variables, bool reverse=false) const;
    Match Matches(const Expression& expression, const Variables& variables, bool reverse=false) const;
    Expression Substitute(const Variables& substitutions) const;
    Expression Infer(const class Knowledge& k, const Variables& substitutions) const;
private: 
    std::string name;
};


std::ostream& operator<<(std::ostream& os, const Tuple& id);

}
