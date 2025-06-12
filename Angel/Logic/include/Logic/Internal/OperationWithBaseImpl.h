#pragma once
#include "Logic/Expression.h"
#include "Logic/Internal/BinaryImpl.h"
#include <numeric>

namespace Angel::Logic
{

template<class T> 
Expression OperationWithBase<T>::Simplify() const
{
    T simple = FlatCollection<T>::SimplifyItems();
    if (simple.empty())
        return T::initial;
    auto it = simple.begin();
    auto const_base = it->template TryCast<Integer>();
    Expression constant(T::initial);
    if (const_base)
    {
        constant = *const_base;
    }
    ++it;
    while(it!=simple.end())
    {
        auto integer = it->template TryCast <Integer>();        
        if (integer)
        {
            constant = T::ope(constant, *integer);
            it = simple.erase(it);
        }
        else 
            ++it;
    }
    if (const_base)
        simple.front() = constant;
    else if (constant!=T::initial) 
        simple.push_back(constant);
    if (simple.size()==1)
        return simple.front();
    else
        return std::move(simple);
}    

template<class T>
Expression OperationWithBase<T>::Matches(const Expression& expression, const Hypothesis& hypothesis) const
{
    // TODO: Operation matches with mathematical simplication
    // X+1 matches 3 if X is 2 
    // all unknown variables need to be found and the remaining expression(s) needs to be computed. 
    // if any are ranges, then the result might be a different range 
    return Boolean(false);
}

template<class T>
Expression OperationWithBase<T>::Infer(const class Knowledge& k, const Hypothesis& hypothesis, Trace& trace) const
{
    if (FlatCollection<T>::empty()) {
        return Integer(T::initial);
    }
    Expression value = std::accumulate(FlatCollection<T>::begin()+1, FlatCollection<T>::end(), 
        FlatCollection<T>::front(),
        [&k, &hypothesis, &trace](const Expression& accumulated, const Expression& item)
        {
            auto inferred = item.Infer(k, hypothesis, trace);
            return T::ope(accumulated, inferred);
        });
    return value;
}    

template<class T>
std::ostream& operator<<(std::ostream& os, const OperationWithBase<T>& operation)
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