#include "Logic/Equation.h"
#include "Logic/Knowledge.h"
#include "Logic/Expression.h"
#include <ranges>
#include <cassert>

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

Expression Equation::Matches(const Expression& exp, const Hypothesis& hypothesis) const
{
    assert(size()==1);  // single element equations can be matched but not inferred. Predicate argument only
    // TODO: Equaiton match with logical simplication
    // true & X matches true if X is true. 
    return front().Matches(exp, hypothesis);
}

Equation Equation::Substitute(const Hypothesis& hypothesis) const
{
    return SubstituteItems(hypothesis);
}

Expression Equation::Infer(const Knowledge& k, const Hypothesis& hypothesis, Trace& trace) const
{
    assert(size()>1);   // single element equations can't be inferred only matched
    if (Assumptions()) {
        return Association(Boolean(true), *this);
    }
    Expression first = front().Infer(k, hypothesis, trace);
    for(const auto& item: *this | std::views::drop(1))
    {
        auto inferred = item.Infer(k, hypothesis, trace);
        auto cast = inferred.Cast(first.AlternativeTypeId());
        // TODO: is matching the same as being equal? 
        // or do they have to be the same type or castable to the same type left or right way around
        // and how does order (not) matter then?  "23" = 23 = 23.0 those should pbly not be automatically equal 
        // but integer and float? 23.0=23   and 23=23.0 ? 
        // at least hypothesis would have to be done here. 
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