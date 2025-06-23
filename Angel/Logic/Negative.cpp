#include "Logic/Negative.h"
#include "Logic/Expression.h"

namespace Angel::Logic
{

Expression Negative::Simplify() const
{
    auto simple = content->Simplify();
    if (const Negative* neg = simple.GetIf<Negative>()) 
    {
        return (*neg)->Simplify();
    }
    else if (const Integer* integer = simple.GetIf<Integer>())
    {
        return ope(*integer);
    }
    return Negative{std::move(simple)};
}

Expression Negative::Matches(const Expression& expression, const Hypothesis& hypothesis) const
{
    // TODO: need to compute with remaining variables, then compare 
    return Boolean(false);
}

Negative Negative::Substitute(const Hypothesis& hypothesis) const
{
    return Negative(content->Substitute(hypothesis));
}

Expression Negative::Infer(const class Knowledge& k, const Hypothesis& hypothesis, Trace& trace) const
{
    auto value = content->Infer(k, hypothesis, trace);
    return ope(value);
}

bool Negative::operator==(const Negative& other) const
{
    return Individual::operator==(other);
}


std::size_t Negative::Hash() const
{
    return Individual::Hash() ^ typeid(*this).hash_code();
}

std::ostream& operator<<(std::ostream& os, const Negative& neg)
{
    os << neg.ope << *neg;
    return os;
}
}