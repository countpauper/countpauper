#pragma once
#include "Logic/FlatCollection.h"
#include "Logic/Substitution.h"
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
    Expression Matches(const Expression& expression, const Substitutions& substitutions) const;
    Equation Substitute(const Substitutions& substitutions) const;
    Expression Infer(const class Knowledge& k, const Substitutions& substitutions) const;
    bool operator==(const Equation& other) const;

    constexpr static BinaryOperator ope {L'='};
};

std::ostream& operator<<(std::ostream& os, const Equation& );


}