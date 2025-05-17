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

Match Disjunction::Matches(const Expression&, const Variables& vars) const
{
    // TODO: Disjunction match with logical simplication
    // false & X matches true if X is true. 
    return NoMatch;
}

Object Disjunction::Infer(const Knowledge& k, const Variables& substitutions) const
{
    for(const auto& item: *this)
    {
        Object inferred = k.Infer(item);
        auto isTrue = inferred.Cast<Boolean>();
        if (isTrue)
            return inferred;
    }
    return Boolean(false);
}

std::size_t Disjunction::Hash() const
{
    return typeid(decltype(*this)).hash_code() ^ Collection::Hash();
}


std::ostream& operator<<(std::ostream& os, const Disjunction& disjunction)
{
    bool first = true;
    for(const auto& obj: disjunction)
    {
        if (!first)
            os << disjunction.ope;
        os << obj;
        first = false;
    }
    return os;
}

}