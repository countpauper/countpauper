#include "Logic/List.h"
#include "Logic/Expression.h"
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

Expression List::Infer(const Knowledge& knowledge, const Variables& substitutions) const
{
    List result;
    std::transform(begin(), end(), std::back_inserter(result), 
        [&knowledge, &substitutions](const Expression& item)
        {
            return item.Infer(knowledge, substitutions);
        });
    return result;
}

Match List::Matches(const Expression& expression, const Variables& variables) const
{
    const List* list = std::get_if<List>(&expression);
    if (!list)
        return Boolean(false);

    if (size()!=list->size())
        return Boolean(false); // TODO: head|tail matching or whatever could happen here 
    auto lit = list->begin();
    Variables vars = variables;
    for(auto it=begin(); it!=end(); ++it, ++lit)
    {
        auto itemMatch = it->Matches(*lit, vars);
        if (itemMatch==Boolean(false))
            return std::move(itemMatch); // early out 
        if (itemMatch==Boolean(true))   
            continue;   // trivial 
        vars.emplace_back(std::move(itemMatch));
    }
    return vars;
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

