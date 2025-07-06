#pragma once
#include "Logic/Internal/FlatTuple.h"
#include <iostream>

namespace Angel::Logic
{

template<class T>
class OperationWithBase : public FlatTuple<T>
{
public:
    OperationWithBase(std::initializer_list<Expression> items) : 
        FlatTuple<T>(items)
    {
        if (items.size() == 0)
            throw std::invalid_argument("Operands list can not be empty");
    }
    template <std::ranges::input_range R>
	explicit OperationWithBase(R items) : FlatTuple<T>(items)
    {
        if (items.size() == 0)
            throw std::invalid_argument("Operands list can not be empty");
    }     
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