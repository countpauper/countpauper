#include "Logic/Conjunction.h"
#include "Logic/Boolean.h"
#include "Logic/Knowledge.h"

namespace Angel::Logic
{

bool Conjunction::operator==(const Conjunction& rhs) const
{
    return Collection::operator==(rhs);
}

// TODO: Compute should return an element
Element Conjunction::Compute(const Knowledge& k, const Variables& substitutions) const
{
    for(const auto& item: *this)
    {
        auto element = k.Compute(item);
        if (!element)
            return Boolean(false); // or just element? true & 0 == 0?
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