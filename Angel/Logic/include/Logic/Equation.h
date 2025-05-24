#pragma once
#include "Logic/FlatCollection.h"
#include "Logic/Match.h"
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
    Match Matches(const Expression& expression, const Variables& vars) const;
    Expression Infer(const class Knowledge& k, const Variables& substitutions) const;
    bool operator==(const Equation& other) const;

    constexpr static BinaryOperator ope {L'='};
};

std::ostream& operator<<(std::ostream& os, const Equation& );


}