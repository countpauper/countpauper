#include "Logic/Conjunction.h"
#include "Logic/Boolean.h"
#include "Logic/Knowledge.h"
#include "Logic/Expression.h"

namespace Angel::Logic
{

bool Conjunction::operator==(const Conjunction& rhs) const
{
    return FlatCollection<Conjunction>::operator==(rhs);
}

Match Conjunction::Matches(const Expression&, const Variables& vars) const
{
    // TODO: Conjunctions match with logical simplication
    // true & X matches true if X is true. 
    return Boolean(false);
}

Expression Conjunction::Infer(const Knowledge& k, const Variables& substitutions) const
{
    for(const auto& item: *this)
    {
        auto inferred = item.Infer(k, substitutions);
        auto isTrue = inferred.Cast<Boolean>();
        if (!isTrue)
            return isTrue; // or just inferred? true & 0 == 0?
    }
    return Boolean(true);
}

std::ostream& operator<<(std::ostream& os, const Conjunction& conjunction)
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