#pragma once 
#include "Logic/Internal/Tuple.h"
#include <algorithm>
#include <ranges>

namespace Angel::Logic
{
Expression Simplify(const Expression& e);
class Set;

// A flat tuple can't contain another collection
// When one is added, it will instead append all its elements.
// This is useful for operations where, for instance, (A+(B+C)) == A+B+C 
template<class T>
class FlatTuple : public Tuple 
{
public: 
    using value_type = Expression;

    FlatTuple() = default;
	FlatTuple(std::initializer_list<Expression> items);

    template <std::ranges::input_range R>
	explicit FlatTuple(R items);
    ~FlatTuple() = default;
    unsigned Add(Expression&& exp);
    unsigned AddAt(const_iterator& at, Expression&& exp);
protected:
    T SimplifyItems() const
    {
        return T(Tuple::SimplifyItems()); 
    }

    T SubstituteItems(const Hypothesis& hypothesis) const
    {
        return T(Tuple::SubstituteItems(hypothesis));
    }
};

    
}