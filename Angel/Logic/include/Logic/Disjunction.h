#pragma once
#include "Logic/FlatCollection.h"
#include "Logic/Substitution.h"
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
    Expression Matches(const Expression& expression, const Substitutions& substitutions) const;
    Expression Substitute(const Substitutions& substitutions) const;
    Expression Infer(const class Knowledge& k, const Substitutions& substitutions) const;
    bool operator==(const Disjunction& other) const;

    static constexpr BinaryOperator ope{L'|'};
};

std::ostream& operator<<(std::ostream& os, const Disjunction& );


}