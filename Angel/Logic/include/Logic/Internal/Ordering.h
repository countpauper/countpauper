#pragma once
#include "Logic/Comparator.h"
#include "Logic/Internal/Comparison.h"
#include "Logic/Internal/Variant.h"
#include "Logic/Equal.h"
#include "Logic/Unequal.h"
#include "Logic/Lesser.h"
#include "Logic/LesserEqual.h"
#include "Logic/Greater.h"
#include "Logic/GreaterEqual.h"
#include <iostream>

namespace Angel::Logic
{

using OrderingVariant = Variant<
    Equal, Unequal, Lesser, LesserEqual, Greater, GreaterEqual>;  

class Ordering : public OrderingVariant
{
public:
    Ordering(const Ordering&) = default;
    using OrderingVariant::OrderingVariant;
    
    Ordering& operator=(const Ordering& e);

    std::size_t size() const;
    Expression Simplify() const;
    Set Assumptions() const;
    bool HasLeftAssumption() const;
    Ordering Substitute(const Hypothesis& hypothesis) const;
    Expression Matches(const Expression& e, const Hypothesis& hypothesis) const;
    Expression Infer(const class Knowledge& knowledge, const Hypothesis& hypothesis, Trace& trace) const;
    Order GetOrder() const;
    std::string Summary() const;

    template<typename T> 
    requires is_alternative<T, OrderingVariant>
    bool operator==(const T& rhs) const
    {
        return std::visit(overloaded_visit{
            [&rhs](const T& lv)
            {
                return lv == rhs;
            }, 
            [](const auto&) { return false; }   
            },*this);
    }
    bool operator==(const Ordering& rhs) const;
    operator bool() const;
    template<typename T> 
    bool operator<(const T& rhs) const
    {
        return std::visit(
            [this, &rhs](const auto& obj) 
            {   
                std::hash<Expression> hasher;
                return this->Hash() < hasher(rhs);
            },*this);
    }

    bool operator<(const Ordering& o) const;
};

std::ostream& operator<<(std::ostream& s, const Ordering& e);

}