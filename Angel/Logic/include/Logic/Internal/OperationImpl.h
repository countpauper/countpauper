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
        auto front = simple.cbegin();
        simple.AddAt(front, std::move(constant));
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
    assert(!FlatTuple<T>::empty());    // must have base, shoukd have been caught at construction

    Expression value = std::accumulate(FlatTuple<T>::begin()+1, FlatTuple<T>::end(), 
        FlatTuple<T>::front().Infer(k, hypothesis, trace),
        [&k, &hypothesis, &trace](const Expression& accumulated, const Expression& item)
        {
            auto inferred = item.Infer(k, hypothesis, trace);
            return T::ope(accumulated, inferred);
        });
    return value;
}    

template<class T>
std::ostream& operator<<(std::ostream& os, const OperationBase<T>& operation)
{
    bool first = true;
    for(const auto& obj: operation)
    {
        auto s = to_string(obj);
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