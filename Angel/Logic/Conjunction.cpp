#include "Logic/Conjunction.h"
#include "Logic/Boolean.h"
#include "Logic/Knowledge.h"
#include "Logic/Expression.h"

namespace Angel::Logic
{

Expression Conjunction::Infer(const Knowledge& k, const Hypothesis& hypothesis, Trace& trace) const
{
    Expression result;
    for(const auto& item: *this)
    {
        Expression inferred = item.Infer(k, hypothesis, trace);
        if (result == identity)
        {
            auto isTrue = inferred.Simplify().Cast<Boolean>();
            if (isTrue == absorb)
                return isTrue;
        }
        if (result)
            result = ope(result, inferred);
        else 
            result = inferred;
    }
    if (!result)
        return identity;
    return result;    
}

}