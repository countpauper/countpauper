#include "Logic/Disjunction.h"
#include "Logic/Boolean.h"
#include "Logic/Object.h"
#include "Logic/Knowledge.h"

namespace Angel::Logic
{

bool Disjunction::operator==(const Disjunction& rhs) const
{
    return Collection::operator==(rhs);
}

Match Disjunction::Matches(const Object& object, const Variables& vars) const
{
    // TODO: Disjunction match with logical simplication
    // false & X matches true if X is true. 
    return NoMatch;
}

Object Disjunction::Infer(const Knowledge& k, const Variables& substitutions) const
{
    for(const auto& item: *this)
    {
        auto object = k.Infer(item);
        auto isTrue = object.Cast<Boolean>();
        if (isTrue)
            return isTrue; // or just object? false | 1 == 1?
    }
    return Boolean(false);
}

std::size_t Disjunction::Hash() const
{
    return typeid(decltype(*this)).hash_code() ^ Collection::Hash();
}


std::ostream& operator<<(std::ostream& os, const Disjunction& list)
{
    bool first = true;
    for(const auto& obj: list)
    {
        if (!first)
            os << "|";
        os << obj;
        first = false;
    }
    return os;
}

}