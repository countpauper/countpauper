#include "Logic/List.h"
#include "Logic/Object.h"
#include <typeinfo>
#include <iostream>

namespace Angel::Logic
{
    

List::operator bool() const
{
    return !empty();
}

bool List::operator==(const List& rhs) const
{
    return Collection::operator==(rhs);
}

Object List::Compute(const Knowledge& knowledge, const Variables& substitutions) const
{
    List result;
    std::transform(begin(), end(), std::back_inserter(result), 
        [&knowledge, &substitutions](const Object& o)
        {
            return o.Compute(knowledge, substitutions);
        });
    return result;
}

Match List::Matches(const List& list) const
{
    if (size()!=list.size())
        return NoMatch; // TODO: head|tail matching or whatever could happen here 
    auto lit = list.begin();
    Variables vars;
    for(auto it=begin(); it!=end(); ++it, ++lit)
    {
        if (*it != *lit)
            return NoMatch; // TODO should match each node, variable substituations etc
        /*
        auto match = it->value.Match(lit->value);
        if (!match)
            return match;
        vars.insert(*match);
        */
    }
    return vars;
}

std::size_t List::Hash() const
{
    return typeid(decltype(*this)).hash_code() ^  Collection::Hash();
}


std::ostream& operator<<(std::ostream& os, const List& list)
{
    bool first = true;
    os << "[";
    for(const auto& obj: list)
    {
        if (!first)
            os << ",";
        os << obj;
        first = false;
    }
    os << "]";
    return os;
}
}

