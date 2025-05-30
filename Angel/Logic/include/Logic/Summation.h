#pragma once
#include "Logic/FlatCollection.h"
#include "Logic/Hypothesis.h"
#include "Logic/Element.h"
#include "Logic/Operator.h"
#include <iostream>

namespace Angel::Logic
{

// A Summation is a mathemetatical operator, which returns all values added together
// It is currently a Collection (ordered, non-unique), but technically it could be unordered 
class Summation : public FlatCollection<Summation>  
{
public:
    using FlatCollection<Summation>::FlatCollection;
    Expression Simplify() const;
    Expression Matches(const Expression& expression, const Hypothesis& hypothesis) const;
    Summation Substitute(const Hypothesis& hypothesis) const;
    Expression Infer(const class Knowledge& k, Hypothesis& hypothesis) const;
    bool operator==(const Summation& other) const;

    constexpr static BinaryOperator ope{L'+'};
};

std::ostream& operator<<(std::ostream& os, const Summation& );


}