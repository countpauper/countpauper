#include "Logic/Conjunction.h"
#include "Logic/Boolean.h"
#include "Logic/Knowledge.h"
#include "Logic/Expression.h"

namespace Angel::Logic
{

bool Conjunction::operator==(const Conjunction& rhs) const
{
    return FlatTuple<Conjunction>::operator==(rhs);
}

Expression Conjunction::Simplify() const
{
    Conjunction simpler = SimplifyItems();
    simpler.Remove(True);
    for(const auto& item: simpler)
    {
        if (item == False)
            return item;
    }
     if (simpler.empty())
        return True;
    else if (simpler.size()==1)
        return simpler.front();
    return simpler;
}

Expression Conjunction::Matches(const Expression&, const Hypothesis& hypothesis) const
{
    // TODO: Conjunctions match with logical simplication
    // true & X matches true if X is true. 
    return False;
}

Conjunction Conjunction::Substitute(const Hypothesis& hypothesis) const
{
    return SubstituteItems(hypothesis);
}

Expression Conjunction::Infer(const Knowledge& k, const Hypothesis& hypothesis, Trace& trace) const
{
    Expression result;
    for(const auto& item: *this)
    {
        Expression inferred = item.Infer(k, hypothesis, trace);
        if (result == True)
        {
            auto isTrue = inferred.Simplify().Cast<Boolean>();
            if (!isTrue)
                return isTrue;
        }
        if (result)
            result = ope(result, inferred);
        else 
            result = inferred;
    }
    if (!result)
        return True;
    return result;    
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