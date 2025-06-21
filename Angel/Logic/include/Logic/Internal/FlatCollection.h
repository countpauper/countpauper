#pragma once 
#include "Logic/Internal/Collection.h"
#include <algorithm>
#include <ranges>

namespace Angel::Logic
{
Expression Simplify(const Expression& e);
class Set;

// A flat collection can't contain another flatcollection of the same T type. 
// When one is pushed_back, it will instead append all its elements.
// This is useful for operations where, for instance, (A+(B+C)) == A+B+C 
template<class T>
class FlatCollection
{
public: 
    using value_type = Expression;

    FlatCollection() = default;
	FlatCollection(std::initializer_list<Expression> items);
	explicit FlatCollection(std::vector<Expression>&& items);

    template <std::ranges::input_range R>
	explicit FlatCollection(R items);
    ~FlatCollection() = default;

    void push_back(const T& same);
    void push_back(const Expression& exp);
    void emplace_back(T&& same);
    void emplace_back(Expression&& exp);

    Set Assumptions() const;

    // template<class... Args >
    // void emplace_back( Args&&... args )
    // {
    //     return items.emplace(std::forward<Args>(args)...);
    // }

    std::size_t erase(const Expression& e) { return std::erase(items, e); }
    Collection::const_iterator erase(Collection::const_iterator it) { return items.erase(it); }
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
protected:
    Collection items;
};

    
}