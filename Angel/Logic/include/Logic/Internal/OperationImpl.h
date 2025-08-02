#pragma once
#include "Logic/Expression.h"
#include "Logic/Internal/BinaryImpl.h"
#include "Logic/Internal/MultiaryImpl.h"

#include <numeric>
#include <cassert>

namespace Angel::Logic
{

template<class T> 
Expression OperationBase<T>::Simplify() const
{
    T simple = FlatTuple<T>::SimplifyItems();

    auto it = simple.begin();
    Expression constant;
    while(it!=simple.end())
    {
        if (it->IsConstant())
        {
            if constexpr (has_absorb<T>)
            {
                if (*it == T::absorb)
                    return T::absorb;
            }
            if constexpr (has_identity<T>) 
            {
                if (*it == T::identity)
                {
                    it = simple.erase(it);
                    continue;
                }
            }
            if (constant)
                constant = T::ope(constant, *it);
            else
                constant = *it;
            it = simple.erase(it);
        }
        else 
            ++it;
    }
    if (constant) 
    {
        simple.AddLeft(std::move(constant));
    }
    if constexpr (has_identity<T>) 
    {
        if (simple.empty())
            return T::identity;
    }

    if (simple.size()==1)
        return simple.front();
    else
        return std::move(simple);
}    

template<class T>
Expression OperationBase<T>::Matches(const Expression& expression, const Hypothesis& hypothesis) const
{
    return Equal{Substitute(hypothesis), expression.Substitute(hypothesis)};
}

template<class T>
Expression OperationBase<T>::Infer(const class Knowledge& k, const Hypothesis& hypothesis, Trace& trace) const
{
    if constexpr (has_identity<T>)
    {
        if (FlatTuple<T>::empty())
            return T();        
    }
    
    T result{FlatTuple<T>::front().Infer(k, hypothesis, trace)};
    result.reserve(FlatTuple<T>::size()); 

    std::transform(FlatTuple<T>::begin()+1, FlatTuple<T>::end(), std::back_inserter(result), [&k, &hypothesis, &trace](const Expression& operand)
    {
            return operand.Infer(k, hypothesis, trace);
    });
    return result;
}    

template<class T>
std::string OperationBase<T>::OperandToString(const Expression& e, bool first)
{
    if (T::ope.NeedsBracesAround(e, first))
        return std::format("({})", to_string(e));
    else 
        return to_string(e);
}

template<class T>
std::ostream& operator<<(std::ostream& os, const OperationBase<T>& operation)
{
    bool first = true;
    for(const auto& obj: operation)
    {
        auto s = OperationBase<T>::OperandToString(obj, false);
        if (!first)
            os << T::ope;
        os << s;
        first = false;
    }
    return os;
}


template<typename... T> 
Operator Operator::Find(std::string_view tag)
{
    Operator found;
    if (tag.empty())
        return found;
    (found = (!found? Operator(FindId(tag, T::operands)) : found), ...);
    return found;
}

}