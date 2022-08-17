#include "stdafx.h"
#include "Disjunction.h"
#include "Boolean.h"
#include "Knowledge.h"

namespace Angel::Logic
{

Disjunction::Disjunction(Set&& operands) :
    Nary(std::move(operands))
{
}

Disjunction::Disjunction(Disjunction&& value) :
    Disjunction(std::move(value.operands))
{
}


bool Disjunction::operator==(const Expression& other) const
{
    // TODO: could be in Nary if the type was the same 
    if (auto conjunction = dynamic_cast<const Disjunction*>(&other))
    {
        return operands == conjunction->operands;
    }
    return false;
}

std::string Disjunction::String() const
{
    std::string result;
    for (const auto& condition : operands)
    {
        if (!result.empty())
            result += "|";
        result += condition->String();
    }
    return result;
}

Object Disjunction::Match(const Expression& other) const
{
    return boolean(other == *this);
}

Object Disjunction::Compute(const Knowledge& knowledge) const
{
    for (const auto& condition : operands)
    {
        auto truth = condition.Compute(knowledge);
        if (truth.Trivial())
            return truth;
    }
    return boolean(false);
}


Object Disjunction::Copy() const
{
    return Create<Disjunction>(Set(operands));
}

Object Disjunction::Cast(const std::type_info& t, const Knowledge& k) const
{
    if (t == typeid(Boolean))
    {
        return Compute(k);
    }
    throw CastException<Disjunction>(t);
}

}