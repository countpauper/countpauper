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

Expression Negative::Matches(const Expression& expression, const Substitutions& substitutions) const
{
    // TODO: need to compute with remaining variables, then compare 
    return Boolean(false);
}


Negative Negative::Substitute(const Substitutions& substitutions) const
{
    return Negative(content->Substitute(substitutions));
}

Expression Negative::Infer(const class Knowledge& k, const Substitutions& substitutions) const
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