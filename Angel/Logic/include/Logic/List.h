#pragma once
#include "Logic/Match.h"
#include "Logic/Collection.h"
#include <iostream>

namespace Angel::Logic
{
class Expression;

// A list is an ordered non-unique collection of objects (wrapped in nodes)
class List : public Collection
{
public:
    using Collection::Collection;

    bool operator==(const List& rhs) const;
    explicit operator bool() const;
    Expression Infer(const class Knowledge& knowledge, const Variables& substitutions) const;
    Match Matches(const Expression& other, const Variables& variables) const;

    static constexpr bool unique=false;
    static constexpr bool ordered=true;
};

using Bag = List;  // TODO bag is unordered, but for now just use this

std::ostream& operator<<(std::ostream& os, const List& list);

}
