#pragma once
#include "Logic/Internal/FlatTuple.h"
#include "Logic/Operator.h"
#include "Logic/Hypothesis.h"
#include <iostream>

namespace Angel::Logic
{

template<class T>
class Comparison : public FlatTuple<T>
{
public:
    using FlatTuple<T>::FlatTuple;

    Expression Simplify() const;
    Expression Matches(const Expression& expression, const Hypothesis& hypothesis) const;
    T Substitute(const Hypothesis& hypothesis) const
    {
        return FlatTuple<T>::SubstituteItems(hypothesis);
    }
    Expression Infer(const class Knowledge& k, const Hypothesis& hypothesis, Trace& trace) const;
    bool HasLeftAssumption() const;
};

}