#pragma once
#include "Logic/Hypothesis.h"
#include "Logic/Internal/Tuple.h"
#include <iostream>

namespace Angel::Logic
{
class Expression;
class Container;

// A list is an ordered non-unique collection of objects (wrapped in nodes)
class List : public Tuple
{
public:
    using Tuple::Tuple;

    bool operator==(const List& rhs) const;
	List& operator+=(const Container& rhs);
	List& operator-=(const Container& rhs);
	List& operator&=(const Container& rhs);
	List& operator|=(const Container& rhs);
	List& operator^=(const Container& rhs);    
    explicit operator bool() const;
    List Simplify() const;
    Expression Matches(const Expression& other, const Hypothesis& hypothesis) const;
    List Substitute(const Hypothesis& hypothesis) const;
    Expression Infer(const class Knowledge& knowledge, const Hypothesis& hypothesis, Trace& trace) const;

    static constexpr bool unique=false;
    static constexpr bool ordered=true;
};

using Bag = List; 

List operator+(List lhs, const Container& rhs);
List operator-(List lhs, const Container& rhs);
List operator&(List lhs, const Container& rhs);
List operator|(List lhs, const Container& rhs);
List operator^(List lhs, const Container& rhs);

std::ostream& operator<<(std::ostream& os, const List& list);

}
