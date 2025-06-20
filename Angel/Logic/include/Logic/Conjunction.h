#pragma once
#include "Logic/Internal/FlatCollection.h"
#include "Logic/Hypothesis.h"
#include "Logic/Element.h"
#include "Logic/Binary.h"
#include <iostream>

namespace Angel::Logic
{

// A Conjunction is a logical operator, which is true, only if all its elements are True
// It is currently a Collection (ordered, non-unique) because the ordering is used to determine lazy evaulation
// but technically it could be ordered and unique. 
class Conjunction : public FlatCollection<Conjunction> 
{
public:
    using FlatCollection<Conjunction>::FlatCollection;
    Expression Simplify() const;
    Expression Matches(const Expression& expression, const Hypothesis& hypothesis) const;
    Conjunction Substitute(const Hypothesis& hypothesis) const;
    Expression Infer(const class Knowledge& k, const Hypothesis& hypothesis, Trace& trace) const;
    bool operator==(const Conjunction& other) const;

    constexpr static BinaryOperator ope {L'∧'};
};

std::ostream& operator<<(std::ostream& os, const Conjunction& );


}