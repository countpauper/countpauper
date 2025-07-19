#pragma once
#include "Logic/Hypothesis.h"
#include "Logic/Binary.h"
#include "Logic/Internal/Pair.h"

namespace Angel::Logic
{

class Expression;
class Operation;

class Item : public Pair
{
public:
    Item(const Item& o): Pair(o) {}
    Item(Item&& o) : Pair(std::move(o)) {}
    using Pair::Pair;

    Item& operator=(const Item& o);
    Item& operator=(Item&& o);

    Expression Simplify() const;
    Expression Matches(const Expression& expression, const Hypothesis& hypothesis) const;
    Item Substitute(const Hypothesis& hypothesis) const;
    Expression Infer(const class Knowledge& k, const Hypothesis& hypothesis, Trace& trace) const;
    bool operator==(const Item& other) const { return Pair::operator==(other); }
    constexpr static BinaryOperator ope{L'.'};
    using Pariant = Operation;
};


std::ostream& operator<<(std::ostream& os, const Item& a);

}