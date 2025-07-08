#pragma once
#include "Logic/Expression.h"
#include "Logic/Internal/BinaryImpl.h"
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
            if constexpr (has_final<T>)
            {
                if (*it == T::final)
                    return T::final;
            }
            if constexpr (has_initial<T>) 
            {
                if (*it == T::initial)
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
    if constexpr (has_initial<T>) 
    {
        if (simple.empty())
            return T::initial;
    }

    if (simple.size()==1)
        return simple.front();
    else
        return std::move(simple);
}    

template<class T>
Expression OperationBase<T>::Matches(const Expression& expression, const Hypothesis& hypothesis) const
{
    // TODO: Operation matches with mathematical simplication
    // X+1 matches 3 if X is 2 
    // all unknown variables need to be found and the remaining expression(s) needs to be computed. 
    // if any are ranges, then the result might be a different range 
    return False;
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

}