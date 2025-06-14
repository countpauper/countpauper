#pragma once
#include "Logic/Internal/FlatCollection.h"
#include "Logic/Hypothesis.h"
#include <iostream>

namespace Angel::Logic
{

template<class T>
class Operation : public FlatCollection<T>
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
protected:
    using BaseType = Operation<T>;
};


template<typename T>
std::ostream& operator<<(std::ostream& os, const Operation<T>& operation);

}