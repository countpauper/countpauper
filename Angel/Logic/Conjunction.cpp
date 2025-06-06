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

Expression Conjunction::Simplify() const
{
    Conjunction simpler = SimplifyItems();
    simpler.erase(Boolean(true));
    for(const auto& item: simpler)
    {
        if (item == Boolean(false))
            return item;
    }
     if (simpler.empty())
        return Boolean(true);
    else if (simpler.size()==1)
        return simpler.front();
    return simpler;
}

Expression Conjunction::Matches(const Expression&, const Hypothesis& hypothesis) const
{
    // TODO: Conjunctions match with logical simplication
    // true & X matches true if X is true. 
    return Boolean(false);
}

Conjunction Conjunction::Substitute(const Hypothesis& hypothesis) const
{
    return SubstituteItems(hypothesis);
}

Expression Conjunction::Infer(const Knowledge& k, const Hypothesis& hypothesis, Trace& trace) const
{
    for(const auto& item: *this)
    {
        auto inferred = item.Infer(k, hypothesis, trace);
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