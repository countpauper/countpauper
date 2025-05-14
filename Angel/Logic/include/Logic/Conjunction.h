#pragma once
#include "Logic/Collection.h"
#include "Logic/Match.h"
#include "Logic/Element.h"
#include <iostream>
namespace Angel::Logic
{

// A Conjunction is a logical operator, which is true, only if all its elements are True
// It is currently a Collection (ordered, non-unique) because the ordering is used to determine lazy evaulation
// but technically it could be ordered and unique. 
class Conjunction : public Collection 
{
public:
    using Collection::Collection;
    Match Matches(const Object& object, const Variables& vars) const;
    Object Infer(const class Knowledge& k, const Variables& substitutions={}) const;
    bool operator==(const Conjunction& other) const;
    std::size_t Hash() const;
};

std::ostream& operator<<(std::ostream& os, const Conjunction& );


}