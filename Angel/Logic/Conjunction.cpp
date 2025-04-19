#include "Logic/Conjunction.h"
#include "Logic/Boolean.h"
#include "Logic/Knowledge.h"

namespace Angel
{
namespace Logic
{




Conjunction::Conjunction(const Operands& value) :
    Nary(value)
{
}

Conjunction::Conjunction(Conjunction&& value) :
    Conjunction(std::move(value.operands))
{
}

bool Conjunction::operator==(const Expression& other) const
{
    // TODO: could be in Nary if the type was the same 
    if (auto conjunction = dynamic_cast<const Conjunction*>(&other))
    {
        return operands == conjunction->operands;
    }
    return false;
}

std::ostream& operator<<(std::ostream& os, const Conjunction& element)
{
    bool first = true;
    for (const auto& condition : element.operands)
    {
        if (first)
            first = false;
        else
            os << " & ";
        os << condition;
    }
    return os;
}

Object Conjunction::Match(const Expression& other) const
{
    return boolean(other == *this);
}

Object Conjunction::Infer(const Knowledge& knowledge) const
{
    for (const auto& condition : operands)
    {
        auto truth = condition.Infer(knowledge);
        if (!truth)
            return truth;
    }
    return boolean(true);
}


Object Conjunction::Copy() const
{
    return Create<Conjunction>(operands);
}

Object Conjunction::Cast(const std::type_info& t, const Knowledge& k) const
{
    if (t == typeid(Boolean))
    {
        return Infer(k);
    }
    throw CastException<Conjunction>(t);
}

}
}