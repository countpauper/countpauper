#include "Logic/Negation.h"
#include "Logic/Expression.h"

namespace Angel::Logic
{

Expression Negation::Simplify() const
{
    auto simple = content->Simplify();
    if (const Negation* neg = simple.GetIf<Negation>()) 
    {
        return (*neg)->Simplify();
    }
    else if (const Boolean* boolean = simple.GetIf<Boolean>())
    {
        return ope(*boolean);
    }
    return Negation{std::move(simple)};
}

Expression Negation::Matches(const Expression& expression, const Hypothesis& hypothesis) const
{
    auto subst = expression.Substitute(hypothesis);    
    if (const auto* i = subst.GetIf<Boolean>())
    {
        auto substContent = content->Substitute(hypothesis);
        return Equal{std::move(substContent), ope(*i)};
    }
    else if (const auto* n = subst.GetIf<Negation>())
    {
        auto substContent = content->Substitute(hypothesis);
        return Equal{std::move(substContent), std::move(*n)};
    }
    else 
    {   // this is the default case, the options above just favor less negations in the condition
        auto substNeg= this->Substitute(hypothesis);    
        return Equal{std::move(substNeg), std::move(subst)};
    }    
}

Negation Negation::Substitute(const Hypothesis& hypothesis) const
{
    return Negation(content->Substitute(hypothesis));
}

Expression Negation::Infer(const class Knowledge& k, const Hypothesis& hypothesis, Trace& trace) const
{
    auto value = content->Infer(k, hypothesis, trace);
    return ope(value);
}

bool Negation::operator==(const Negation& other) const
{
    return Individual::operator==(other);
}

std::size_t Negation::Hash() const
{
    return Individual::Hash() ^ typeid(*this).hash_code();
}

std::ostream& operator<<(std::ostream& os, const Negation& neg)
{
    os << neg.ope << Negation::OperandToString(Negation::ope, *neg);
    return os;
}
}