#include "Logic/Disjunction.h"
#include "Logic/Boolean.h"
#include "Logic/Knowledge.h"
#include "Logic/Expression.h"

namespace Angel::Logic
{

bool Disjunction::operator==(const Disjunction& rhs) const
{
    return FlatCollection<Disjunction>::operator==(rhs);
}

Expression Disjunction::Simplify() const
{
    if (empty())
        return Boolean(false);
    else if (size()==1)
        return front().Simplify();
    else 
        return SimplifyItems();
    // TODO: could simplify all Boolean(true) to true or all Boolean(false) 
    //  perhaps even after casting 
}

Match Disjunction::Matches(const Expression&, const Variables& vars) const
{
    // TODO: Disjunction match with logical simplication
    // false & X matches true if X is true. 
    return Boolean(false);
}

Disjunction Disjunction::Substitute(const Variables& substitutions) const
{
    return SubstituteItems(substitutions);
}

Expression Disjunction::Infer(const Knowledge& k, const Variables& substitutions) const
{
    for(const auto& item: *this)
    {
        Expression inferred = item.Infer(k, substitutions);
        auto isTrue = inferred.Cast<Boolean>();
        if (isTrue)
            return inferred;
    }
    return Boolean(false);
}

std::ostream& operator<<(std::ostream& os, const Disjunction& disjunction)
{
    bool first = true;
    for(const auto& obj: disjunction)
    {
        if (!first)
            os << disjunction.ope;
        os << obj;
        first = false;
    }
    return os;
}

}