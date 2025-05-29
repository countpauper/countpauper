#include "Logic/Equation.h"
#include "Logic/Knowledge.h"
#include "Logic/Expression.h"
#include <ranges>

namespace Angel::Logic
{

bool Equation::operator==(const Equation& rhs) const
{
    return FlatCollection<Equation>::operator==(rhs);
}

Expression Equation::Simplify() const
{
    // TODO: perhaps could simplify all items and if they are 
    // the same (after casting) then just return true. if they can't be 
    // trivally cast or simplified to the same type (eg predicates, variables) 
    // then return the original 
    return *this;   
}

// forward declared free simplify function for use in std::transform etc, like in FlatCollection
Expression ExpressionSimplifier(const Expression& e)
{
    return e.Simplify();
}

Expression Equation::Matches(const Expression&, const Substitutions& substitutions) const
{
    // TODO: Equaiton match with logical simplication
    // true & X matches true if X is true. 
    return Boolean(false);
}

Equation Equation::Substitute(const Substitutions& substitutions) const
{
    return SubstituteItems(substitutions);
}

Expression Equation::Infer(const Knowledge& k, const Substitutions& substitutions) const
{
    if (size()<2)
        return Boolean(true);
    Expression first = front().Infer(k, substitutions);
    for(const auto& item: *this | std::views::drop(1))
    {
        auto inferred = item.Infer(k, substitutions);
        auto cast = inferred.Cast(first.AlternativeTypeId());
        // TODO: is matching the same as being equal? 
        // or do they have to be the same type or castable to the same type left or right way around
        // and how does order (not) matter then?  "23" = 23 = 23.0 those should pbly not be automatically equal 
        // but integer and float? 23.0=23   and 23=23.0 ? 
        // at least substitutions would have to be done here. 
        if (first != cast)
            return Boolean(false);
    }
    return Boolean(true);
}

// TODO: Generalize with operation 
std::ostream& operator<<(std::ostream& os, const Equation& conjunction)
{
    bool first = true;
    for(const auto& obj: conjunction)
    {
        if (!first)
            os << conjunction.ope;
        os << obj;
        first = false;
    }
    return os;
}

}