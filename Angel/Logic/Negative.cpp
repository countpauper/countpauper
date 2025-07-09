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
    auto subst = expression.Substitute(hypothesis);    
    if (const auto* i = subst.GetIf<Integer>())
    {
        auto substContent = content->Substitute(hypothesis);
        return Equal{std::move(substContent), Integer(-**i)};
    }
    else if (const auto* n = subst.GetIf<Negative>())
    {
        auto substContent = content->Substitute(hypothesis);
        return Equal{std::move(substContent), std::move(*n)};
    }
    else 
    {   // this is the default case, the options above just favor less negatives in the condition
        auto substNeg= this->Substitute(hypothesis);    
        return Equal{std::move(substNeg), std::move(subst)};
    }    
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