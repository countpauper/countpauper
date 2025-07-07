#pragma once
#include "Logic/Internal/FlatTuple.h"
#include "Logic/Operator.h"
#include "Logic/Hypothesis.h"
#include <iostream>

namespace Angel::Logic
{

template<Comparator CO>
class Comparison : public FlatTuple<Comparison<CO>>
{
public:
    using FlatTuple<Comparison<CO>>::FlatTuple;

    Expression Simplify() const;
    Expression Matches(const Expression& expression, const Hypothesis& hypothesis) const;
    Comparison<CO> Substitute(const Hypothesis& hypothesis) const
    {
        return FlatTuple<Comparison<CO>>::SubstituteItems(hypothesis);
    }
    Expression Infer(const class Knowledge& k, const Hypothesis& hypothesis, Trace& trace) const;
    bool operator==(const Comparison<CO>& rhs) const { return FlatTuple<Comparison<CO>>::operator==(rhs); }
    bool HasLeftAssumption() const;
    constexpr static Comparator ope = CO;    
};


template < typename T >
concept is_comparison = std::derived_from<decltype(T::ope), Comparator>;

}