#pragma once
#include "Logic/Collection.h"
#include "Logic/Match.h"
#include "Logic/Element.h"
#include "Logic/Operator.h"
#include <iostream>
namespace Angel::Logic
{

// A Disjunction is a logical operator, which is true, if any of its elements is True
// It is currently a Collection (ordered, non-unique) because the ordering is used to determine lazy evaulation
// but technically it could be ordered and unique. 
class Disjunction : public Collection 
{
public:
    using Collection::Collection;
    Match Matches(const Expression& expression, const Variables& vars) const;
    Object Infer(const class Knowledge& k, const Variables& substitutions={}) const;
    bool operator==(const Disjunction& other) const;

    static constexpr Operator ope{L'|'};
};

std::ostream& operator<<(std::ostream& os, const Disjunction& );


}