#include "Logic/Negative.h"
#include "Logic/Expression.h"

namespace Angel::Logic
{

Expression Negative::Simplify() const
{
    if (const Negative* neg = content->GetIf<Negative>()) 
    {
        return (*neg)->Simplify();
    }
    return Negative{content->Simplify()};
}

Match Negative::Matches(const Expression& expression, const Variables& vars) const
{
    // TODO: need to compute with remaining variables, then compare 
    return Boolean(false);
}


Negative Negative::Substitute(const Variables& substitutions) const
{
    return Negative(content->Substitute(substitutions));
}

Expression Negative::Infer(const class Knowledge& k, const Variables& substitutions) const
{
    auto value = content->Infer(k, substitutions);
    auto intValue = value.Cast<Integer>();
    return Integer(- *intValue);
}

bool Negative::operator==(const Negative& other) const
{
    return Individual::operator==(other);
}

std::ostream& operator<<(std::ostream& os, const Negative& neg)
{
    os << neg.ope << *neg;
    return os;
}
}