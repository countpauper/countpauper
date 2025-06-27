#pragma once 
#include "Logic/Hypothesis.h"
#include "Logic/Individual.h"
#include "Logic/Internal/VariantUtils.h"
#include "Logic/Unary.h"
#include "Logic/Id.h"
#include <memory>
#include <vector>

namespace Angel::Logic
{

class Variable;
class Expression;
class const_container_iterator;
using Tuple_subrange = std::ranges::subrange<std::vector<Expression>::const_iterator, 
                                                    std::vector<Expression>::const_iterator>;

class All : public Individual
{
public:
    explicit All(const std::string_view name);
    All(std::initializer_list<Expression> items);
    explicit All(Expression&& e);
    using Individual::Individual;

    All Simplify() const;
    Expression Matches(Tuple_subrange range, const Hypothesis& substituions, bool reverse=false) const;
    Expression Matches(const Expression& expression, const Hypothesis& substutions, bool reverse=false) const;
    All Substitute(const Hypothesis& hypothesis) const;
    Expression Infer(const class Knowledge& k, const Hypothesis& hypothesis, Trace& trace) const;
    bool operator==(const All& other) const;
    std::size_t size() const;
    std::optional<Variable> GetVariable() const;
    Set Assumptions() const;
    const_container_iterator begin() const;
    const_container_iterator end() const;
    constexpr static PrefixOperator ope{L'∀'};
};

std::ostream& operator<<(std::ostream& os, const All& );

}