#pragma once
#include "Logic/FlatCollection.h"
#include "Logic/Match.h"
#include "Logic/Element.h"
#include "Logic/Operator.h"
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
    Match Matches(const Expression& expression, const Variables& vars) const;
    Expression Infer(const class Knowledge& k, const Variables& substitutions) const;
    bool operator==(const Conjunction& other) const;

    constexpr static BinaryOperator ope {L'&'};
};

std::ostream& operator<<(std::ostream& os, const Conjunction& );


}