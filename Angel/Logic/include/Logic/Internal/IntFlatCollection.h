#include "Logic/Internal/FlatCollection.h"
#include "Logic/Expression.h"

namespace Angel::Logic
{
template<typename T>
FlatCollection<T>::FlatCollection(std::initializer_list<Expression> items)
{
    for(const auto& item: items)
    {
        if (const T* same = item.GetIf<T>())
        {
            push_back(*same);
        }
        else 
        {
            push_back(item);
        }
    }
}

template<typename T>
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

template<typename T>
template <std::ranges::input_range R>
FlatCollection<T>::FlatCollection(R items)
{
    for(auto& item: items)
    {
        if (const T* same = item.template GetIf<T>())
        {
            push_back(*same);
        }
        else 
        {
            push_back(item);
        }
    }
}

template<typename T>
void FlatCollection<T>::emplace_back(Expression&& exp)
{
    if (!bool(exp))
        return;
    if (T* same = exp.GetIf<T>())
    {
        emplace_back(std::move(*same));
    }
    else 
    {
        items.emplace_back(exp);
    }
}

template<typename T>
void FlatCollection<T>::push_back(const T& same)
{
    items.insert(items.end(), same.begin(), same.end());
}

template<typename T>
void FlatCollection<T>::push_back(const Expression& exp)
{
    if (!bool(exp))
        return;
    if (const T* same = exp.GetIf<T>())
    {
        push_back(*same);
    }
    else 
    {
        items.push_back(exp);
    }
}

template<typename T>
void FlatCollection<T>::emplace_back(T&& same)
{
    items.insert(items.end(), same.begin(), same.end());
}

template<typename T>
std::size_t FlatCollection<T>::erase(const Expression& e)
{
    return std::erase(items, e);
}

template<typename T>
Collection::const_iterator FlatCollection<T>::erase(Collection::const_iterator it)
{
    return items.erase(it);
}



}
