#include "Logic/Disjunction.h"
#include "Logic/Boolean.h"
#include "Logic/Knowledge.h"

namespace Angel::Logic
{

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

std::ostream& operator<<(std::ostream& os, const Disjunction& element)
{
    bool first = true;
    for (const auto& condition : element.operands)
    {
        if (first)
            first = false;
        else
            os << " | ";
        os << condition;
    }
    return os;
}

Object Disjunction::Simplify() const
{
    if (operands.empty())
    {
        return boolean(false);
    }
    else if (operands.size() == 1)
    {
        return Object(std::move(*operands.begin()));
    }
    else
    {
        return Copy();
    }

}

Object Disjunction::Infer(const Knowledge& knowledge, const Variables& substitutions) const
{
    for (const auto& condition : operands)
    {
        auto truth = condition.Infer(knowledge, substitutions);
        if (truth)
            return truth;
    }
    return boolean(false);
}


Object Disjunction::Copy() const
{
    return Create<Disjunction>(operands);
}

Object Disjunction::Cast(const std::type_info& t, const Knowledge& k) const
{
    if (t == typeid(Boolean))
    {
        return Infer(k, Variables());
    }
    throw CastException<Disjunction>(t);
}

}