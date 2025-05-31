#pragma once 
#include "Logic/Internal/Collection.h"
#include <algorithm>

namespace Angel::Logic
{
Expression Simplify(const Expression& e);

// A flat collection can't contain itself as a type and will automatically attempt 
// all elements. 
template<typename T>
class FlatCollection
{
public: 
    using value_type = Expression;

    FlatCollection() = default;
	FlatCollection(std::initializer_list<Expression> items)
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
	explicit FlatCollection(std::vector<Expression>&& items)
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

    ~FlatCollection() = default;

    void push_back(const T& same)
    {
        items.insert(items.end(), same.begin(), same.end());
    }

    void push_back(const Expression& exp)
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

    void emplace_back(T&& same)
    {
        items.insert(items.end(), same.begin(), same.end());
    }

    void emplace_back(Expression&& exp)
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

    std::size_t erase(const Expression& e)
    {
        return std::erase(items, e);
    }

    Collection::const_iterator erase(Collection::const_iterator it)
    {
        return items.erase(it);
    }

    std::size_t Assumptions() const { return items.Assumptions(); }

    // template<class... Args >
    // void emplace_back( Args&&... args )
    // {
    //     return items.emplace(std::forward<Args>(args)...);
    // }

    std::size_t size() const { return items.size(); }
    bool empty() const { return items.empty(); }
    Collection::const_iterator begin() const { return items.begin(); }
    Collection::const_iterator end() const { return items.end(); }
    const Expression& front() const { return items.front(); }
    Expression& front() { return items.front(); }
    const Expression& back() const { return items.back(); }
    bool operator==(const FlatCollection<T>& rhs) const { return items.operator==(rhs.items); }
    std::size_t Hash() const { return items.Hash(); }
protected:

    T SimplifyItems() const
    {
        return T(items.SimplifyItems()); 
    }

    T SubstituteItems(const Hypothesis& hypothesis) const
    {
        return T(items.SubstituteItems(hypothesis));
    }
private:
    Collection items;
};

    
}