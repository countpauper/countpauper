#pragma once 
#include "Logic/Internal/FlatCollection.h"
#include "Logic/Expression.h"

namespace Angel::Logic 
{

template<class T>
FlatCollection<T>::FlatCollection(std::initializer_list<Expression> items)
{
    for(const auto& item: items)
    {
        if (const T* same = std::get_if<T>(&item))
        {
            push_back(*same);
        }
        else 
        {
            push_back(item);
        }
    }
}

template<class T>
FlatCollection<T>::FlatCollection(std::vector<Expression>&& items)
{
    for(auto& item: items)
    {
        if (const T* same = std::get_if<T>(&item))
        {
            emplace_back(std::move(*same));
        }
        else 
        {
            emplace_back(std::move(item));
        }
    }
}


template <typename T>
template<std::ranges::input_range R>
FlatCollection<T>::FlatCollection(R items)
{
    for(auto& item: items)
    {
        if (const T* same = std::get_if<T>(&item))
        {
            push_back(*same);
        }
        else 
        {
            push_back(item);
        }
    }
}


template<class T>
void FlatCollection<T>::push_back(const T& same)
{
    items.insert(items.end(), same.begin(), same.end());
}

template<class T>
void FlatCollection<T>::push_back(const Expression& exp)
{
    if (!bool(exp))
        return;
    if (const T* same = std::get_if<T>(&exp))
    {
        push_back(*same);
    }
    else 
    {
        items.push_back(exp);
    }
}

template<class T>
void FlatCollection<T>::emplace_back(T&& same)
{
    items.insert(items.end(), same.begin(), same.end());
}

template<class T>
void FlatCollection<T>::emplace_back(Expression&& exp)
{
    if (!bool(exp))
        return;
    if (T* same = std::get_if<T>(&exp))
    {
        emplace_back(std::move(*same));
    }
    else 
    {
        items.emplace_back(exp);
    }
}

template<class T>
Set FlatCollection<T>::Assumptions() const 
{ 
    return items.Assumptions(); 
}

}