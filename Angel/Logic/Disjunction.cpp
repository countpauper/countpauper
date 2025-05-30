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
    Disjunction simpler = SimplifyItems();
    simpler.erase(Boolean(false));
    for(const auto& item: simpler)
    {
        if (item == Boolean(true))
            return item;
    }
     if (simpler.empty())
        return Boolean(false);
    else if (simpler.size()==1)
        return simpler.front();
    return simpler;
}

Expression Disjunction::Matches(const Expression&, const Hypothesis& hypothesis) const
{
    // TODO: Disjunction match with logical simplication
    // false & X matches true if X is true. 
    return Boolean(false);
}

Expression Disjunction::Substitute(const Hypothesis& hypothesis) const
{
    return SubstituteItems(hypothesis);
}

Expression Disjunction::Infer(const Knowledge& k, const Hypothesis& hypothesis, Trace& trace) const
{
    for(const auto& item: *this)
    {
        Expression inferred = item.Infer(k, hypothesis, trace);
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