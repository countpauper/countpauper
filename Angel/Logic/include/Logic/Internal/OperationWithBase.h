#pragma once
#include "Logic/Internal/FlatTuple.h"
#include <iostream>

namespace Angel::Logic
{

template<class T>
class OperationWithBase : public FlatTuple<T>
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
protected:
    using BaseType = OperationWithBase<T>;
};

}