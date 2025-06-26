#pragma once
#include "Logic/Expression.h"
#include "Logic/Internal/UnaryImpl.h"
#include "Logic/Internal/BinaryImpl.h"
#include <numeric>

namespace Angel::Logic
{

template<class T> 
Expression Operation<T>::Simplify() const
{
    T simple = FlatCollection<T>::SimplifyItems();
    auto it = simple.begin();
    Expression constant(T::initial);
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
    if (constant!=T::initial)
        simple.push_back(constant);
    if (simple.empty())
        return Integer(T::initial);
    else if (simple.size()==1)
        return simple.front();
    else
        return std::move(simple);
}    

template<class T>
Expression Operation<T>::Matches(const Expression& expression, const Hypothesis& hypothesis) const
{
    // TODO: Operation matches with mathematical simplication
    // X+1 matches 3 if X is 2 
    // all unknown variables need to be found and the remaining expression(s) needs to be computed. 
    // if any are ranges, then the result might be a different range 
    return False;
}



template<class T>
Expression Operation<T>::Infer(const class Knowledge& k, const Hypothesis& hypothesis, Trace& trace) const
{
    // TODO: float and imaginary and upgrade when needed, also when overflowing
    // this can, for instance, be done by accumulating an Expression and making Objects implement operator+(Expression) etc
    
    Expression value = std::accumulate(FlatCollection<T>::begin(), FlatCollection<T>::end(), Expression(T::initial),
        [&k, &hypothesis, &trace](Expression accumulated, const Expression& item)
        {
            auto inferred = item.Infer(k, hypothesis, trace);
            return T::ope(accumulated, inferred);
        });
    return value;
}    

template<class T>
std::ostream& operator<<(std::ostream& os, const Operation<T>& operation)
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