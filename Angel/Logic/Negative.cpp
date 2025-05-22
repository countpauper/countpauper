#include "Logic/Negative.h"
#include "Logic/Expression.h"

namespace Angel::Logic
{

Match Negative::Matches(const Expression& expression, const Variables& vars) const
{
    // TODO: need to compute with remaining variables, then compare 
    return Boolean(false);
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