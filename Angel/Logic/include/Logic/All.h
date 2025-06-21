#pragma once 
#include "Logic/Hypothesis.h"
#include "Logic/Individual.h"
#include "Logic/Internal/VariantUtils.h"
#include "Logic/Unary.h"
#include <memory>

namespace Angel::Logic
{

class All : public Individual
{
public:
    template<class T> 
    requires is_container<T>
    explicit All(const T& container) : Individual(Expression(container)) {}
    explicit All(const class Tuple& tuple);
    All(std::initializer_list<Expression> items);

    All Simplify() const;
    Expression Matches(const Expression& expression, const Hypothesis& substutions) const;
    All Substitute(const Hypothesis& hypothesis) const;
    Expression Infer(const class Knowledge& k, const Hypothesis& hypothesis, Trace& trace) const;
    bool operator==(const All& other) const;
    std::size_t size() const;
    constexpr static PrefixOperator ope{L'∀'};
};

std::ostream& operator<<(std::ostream& os, const All& );

}