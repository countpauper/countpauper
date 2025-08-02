#pragma once 
#include "Logic/Internal/FlatTuple.h"
#include "Logic/Expression.h"
#include "Logic/Set.h"
#include "Logic/Internal/container_iterator.h"

namespace Angel::Logic 
{

template<class T>
FlatTuple<T>::FlatTuple(std::initializer_list<Expression> items)
{
    for(auto item: items)
    {
        Add(std::move(item));
    }
}

template <typename T>
template<std::ranges::input_range R>
FlatTuple<T>::FlatTuple(R items)
{
    for(auto item: items)
    {
        Add(std::move(item)); 
    }
}

template<class T>
unsigned FlatTuple<T>::Add(Expression&& e)
{
    const_iterator it = end();
    return AddAt(it, std::move(e));
}

template<class T>
unsigned FlatTuple<T>::AddAt(const_iterator& at, Expression&& e)
{
    if (const auto* same = e.GetIf<T>())
    {
        unsigned total = 0;
        for(auto item: *same)
        {
            total += AddAt(at, std::move(item));
            ++at;
        }
        return total;

    }
    return Tuple::AddAt(at, std::move(e));
}

template<class T>
unsigned FlatTuple<T>::AddLeft(Expression&& exp)
{
    const_iterator it = begin();
    return AddAt(it, std::move(exp));
}

template<class T>
Expression FlatTuple<T>::RemoveLeft()
{
    if (empty())
        return Expression();
    Expression result = front();
    erase(begin());
    return result;
}

}