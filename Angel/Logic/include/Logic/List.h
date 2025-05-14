#pragma once
#include "Logic/Match.h"
#include "Logic/Collection.h"
#include <iostream>

namespace Angel::Logic
{

// A list is an ordered non-unique collection of objects (wrapped in nodes)
class List : public Collection
{
public:
    using Collection::Collection;

    bool operator==(const List& rhs) const;
    explicit operator bool() const;
    Object Compute(const class Knowledge& knowledge, const Variables& substitutions) const;
    Match Matches(const Object& other, const Variables& variables) const;
    std::size_t Hash() const;
};

std::ostream& operator<<(std::ostream& os, const List& list);

}
