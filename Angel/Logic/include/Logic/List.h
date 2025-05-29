#pragma once
#include "Logic/Substitution.h"
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
    Expression Simplify() const;
    Expression Matches(const Expression& other, const Substitutions& substitutions) const;
    List Substitute(const Substitutions& substitutions) const;
    Expression Infer(const class Knowledge& knowledge, const Substitutions& substitutions) const;

    static constexpr bool unique=false;
    static constexpr bool ordered=true;
};

using Bag = List;  // TODO bag is unordered, but for now just use this

std::ostream& operator<<(std::ostream& os, const List& list);

}
