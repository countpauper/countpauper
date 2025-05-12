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
    operator bool() const;
    Match Matches(const List& other) const;
    std::size_t Hash() const;
};

std::ostream& operator<<(std::ostream& os, const List& list);

}
