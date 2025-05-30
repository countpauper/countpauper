#pragma once
#include "Logic/FlatCollection.h"
#include "Logic/Hypothesis.h"
#include "Logic/Element.h"
#include "Logic/Operator.h"
#include <iostream>

namespace Angel::Logic
{

class Equation : public FlatCollection<Equation> 
{
public:
    using FlatCollection<Equation>::FlatCollection;
    Expression Simplify() const;
    Expression Matches(const Expression& expression, const Hypothesis& hypothesis) const;
    Equation Substitute(const Hypothesis& hypothesis) const;
    Expression Infer(const class Knowledge& k, const Hypothesis& hypothesis, Trace& trace) const;
    bool operator==(const Equation& other) const;

    constexpr static BinaryOperator ope {L'='};
};

std::ostream& operator<<(std::ostream& os, const Equation& );


}