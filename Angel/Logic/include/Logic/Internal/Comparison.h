#pragma once
#include "Logic/Internal/FlatTuple.h"
#include "Logic/Operator.h"
#include "Logic/Hypothesis.h"
#include <iostream>

namespace Angel::Logic
{

template < typename T >
concept is_comparison = std::derived_from<decltype(T::ope), Comparator> ||          
                        std::derived_from<decltype(T::ope), Order>;


template<class T>
class Comparison : public FlatTuple<T>
{
public:
    using FlatTuple<T>::FlatTuple;

    Expression Simplify() const;
    Expression Matches(const Expression& expression, const Hypothesis& hypothesis) const;
    T Substitute(const Hypothesis& hypothesis) const
    {
        return T(FlatTuple<T>::SubstituteItems(hypothesis));
    }
    Expression Infer(const class Knowledge& k, const Hypothesis& hypothesis, Trace& trace) const;
    bool operator==(const Comparison<T>& rhs) const { return FlatTuple<T>::operator==(rhs); }
    bool HasLeftAssumption() const;
    static std::string OperandToString(const Expression& e, bool first);    
    using Pariant = class Ordering;
};




}