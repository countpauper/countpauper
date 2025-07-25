#include "Logic/Exclusion.h"
#include "Logic/Boolean.h"
#include "Logic/Knowledge.h"
#include "Logic/Expression.h"

namespace Angel::Logic
{

Expression Exclusion::Infer(const Knowledge& k, const Hypothesis& hypothesis, Trace& trace) const
{
    Expression result;
    for(const auto& item: *this)
    {
        Expression inferred = item.Infer(k, hypothesis, trace);
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