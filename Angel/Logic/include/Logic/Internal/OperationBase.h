#pragma once
#include "Logic/Internal/FlatTuple.h"
#include <iostream>

namespace Angel::Logic
{

// If the derived type T has an T::initial, that will be the value of the simplication/iference if the operation has 0 terms 
template<typename T>
concept has_initial = std::convertible_to<decltype(T::initial), Expression>;
// If the derived type T has a T::final, that will be the value if the simplification/inference has at least one final term
// for instance Multiplication{x,0,y} = 0, so T::final = Integer(0). Also for conjunction/disjunction
template<typename T>
concept has_final = std::convertible_to<decltype(T::final), Expression>;


template<class T>
class OperationBase : public FlatTuple<T>
{
public:
    OperationBase(std::initializer_list<Expression> items) : 
        FlatTuple<T>(items)
    {
        if (!has_initial<T> && items.size() == 0)
            throw std::invalid_argument("Operands list can not be empty");
    }
    template <std::ranges::input_range R>
	explicit OperationBase(R items) : FlatTuple<T>(items)
    {
        if (!has_initial<T> && items.size() == 0)
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
    // default constructor protected so derived classes can make it public to allow empty terms
    // The derived classes should implement the case by overloading Infer
    OperationBase() = default; 
protected:
    using BaseType = OperationBase<T>;
};

}