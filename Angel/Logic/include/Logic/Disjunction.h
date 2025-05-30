#pragma once
#include "Logic/FlatCollection.h"
#include "Logic/Hypothesis.h"
#include "Logic/Element.h"
#include "Logic/Operator.h"
#include <iostream>
namespace Angel::Logic
{

// A Disjunction is a logical operator, which is true, if any of its elements is True
// It is currently a Collection (ordered, non-unique) because the ordering is used to determine lazy evaulation
// but technically it could be ordered and unique. 
class Disjunction : public FlatCollection<Disjunction> 
{
public:
    using FlatCollection<Disjunction>::FlatCollection;
    Expression Simplify() const;
    Expression Matches(const Expression& expression, const Hypothesis& hypothesis) const;
    Expression Substitute(const Hypothesis& hypothesis) const;
    Expression Infer(const class Knowledge& k, Hypothesis& hypothesis) const;
    bool operator==(const Disjunction& other) const;

    static constexpr BinaryOperator ope{L'|'};
};

std::ostream& operator<<(std::ostream& os, const Disjunction& );


}