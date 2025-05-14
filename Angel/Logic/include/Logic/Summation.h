#pragma once
#include "Logic/Collection.h"
#include "Logic/Match.h"
#include "Logic/Element.h"
#include <iostream>
namespace Angel::Logic
{

// A Summation is a mathemetatical operator, which returns all values added together
// It is currently a Collection (ordered, non-unique), but technically it could be unordered 
class Summation : public Collection 
{
public:
    using Collection::Collection;
    Match Matches(const Expression& expression, const Variables& vars) const;
    Object Infer(const class Knowledge& k, const Variables& substitutions={}) const;
    bool operator==(const Summation& other) const;
    std::size_t Hash() const;
};

std::ostream& operator<<(std::ostream& os, const Summation& );


}