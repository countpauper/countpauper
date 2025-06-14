#pragma once
#include "Logic/Internal/FlatCollection.h"
#include "Logic/Operator.h"
#include "Logic/Hypothesis.h"
#include <iostream>

namespace Angel::Logic
{

template<class T>
class Comparison : public FlatCollection<T>
{
public:
    using FlatCollection<T>::FlatCollection;

    Expression Simplify() const;
    Expression Matches(const Expression& expression, const Hypothesis& hypothesis) const;
    T Substitute(const Hypothesis& hypothesis) const
    {
        return FlatCollection<T>::SubstituteItems(hypothesis);
    }
    Expression Infer(const class Knowledge& k, const Hypothesis& hypothesis, Trace& trace) const;
    bool HasLeftAssumption() const;
};

}