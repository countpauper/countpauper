#include "Logic/Disjunction.h"
#include "Logic/Boolean.h"
#include "Logic/Knowledge.h"
#include "Logic/Expression.h"

namespace Angel::Logic
{

bool Disjunction::operator==(const Disjunction& rhs) const
{
    return Collection::operator==(rhs);
}

Match Disjunction::Matches(const Expression&, const Variables& vars) const
{
    // TODO: Disjunction match with logical simplication
    // false & X matches true if X is true. 
    return Boolean(false);
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