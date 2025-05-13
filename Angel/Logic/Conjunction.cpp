#include "Logic/Conjunction.h"
#include "Logic/Boolean.h"
#include "Logic/Knowledge.h"

namespace Angel::Logic
{

bool Conjunction::operator==(const Conjunction& rhs) const
{
    return Collection::operator==(rhs);
}

Object Conjunction::Compute(const Knowledge& k, const Variables& substitutions) const
{
    for(const auto& item: *this)
    {
        auto object = k.Compute(item);
        auto isTrue = object.Cast<Boolean>();
        if (!isTrue)
            return isTrue; // or just object? true & 0 == 0?
    }
    return Boolean(true);
}

std::size_t Conjunction::Hash() const
{
    return typeid(decltype(*this)).hash_code() ^ Collection::Hash();
}


std::ostream& operator<<(std::ostream& os, const Conjunction& list)
{
    bool first = true;
    for(const auto& obj: list)
    {
        if (!first)
            os << "&";
        os << obj;
        first = false;
    }
    return os;
}

}