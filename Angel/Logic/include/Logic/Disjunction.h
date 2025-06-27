#pragma once
#include "Logic/Internal/FlatTuple.h"
#include "Logic/Hypothesis.h"
#include "Logic/Element.h"
#include "Logic/Binary.h"
#include <iostream>

namespace Angel::Logic
{

// A Disjunction is a logical operator, which is true, if any of its elements is True
// It is currently a Tuple (ordered, non-unique) because the ordering is used to determine lazy evaulation
// but technically it could be ordered and unique. 
class Disjunction : public FlatTuple<Disjunction> 
{
public:
    using FlatTuple<Disjunction>::FlatTuple;
    Expression Simplify() const;
    Expression Matches(const Expression& expression, const Hypothesis& hypothesis) const;
    Expression Substitute(const Hypothesis& hypothesis) const;
    Expression Infer(const class Knowledge& k, const Hypothesis& hypothesis, Trace& trace) const;
    bool operator==(const Disjunction& other) const;

    static constexpr BinaryOperator ope{L'∨'};
};

std::ostream& operator<<(std::ostream& os, const Disjunction& );


}