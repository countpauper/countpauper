#pragma once
#include "Logic/Hypothesis.h"
#include "Logic/Internal/Collection.h"
#include <iostream>

namespace Angel::Logic
{
class Expression;

// A list is an ordered non-unique collection of objects (wrapped in nodes)
class List : public Collection
{
public:
    using Collection::Collection;

    bool operator==(const List& rhs) const;
	List& operator+=(const List& rhs);
	List& operator-=(const List& rhs);
	List& operator&=(const List& rhs);
	List& operator|=(const List& rhs);
    explicit operator bool() const;
    List Simplify() const;
    Expression Matches(const Expression& other, const Hypothesis& hypothesis) const;
    List Substitute(const Hypothesis& hypothesis) const;
    Expression Infer(const class Knowledge& knowledge, const Hypothesis& hypothesis, Trace& trace) const;

    static constexpr bool unique=false;
    static constexpr bool ordered=true;
};

using Bag = List;  // TODO bag is unordered, but for now just use this

List operator+(List lhs, const List& rhs);
List operator-(List lhs, const List& rhs);
List operator&(List lhs, const List& rhs);
List operator|(List lhs, const List& rhs);

std::ostream& operator<<(std::ostream& os, const List& list);

}
