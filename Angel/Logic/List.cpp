#include "Logic/List.h"
#include "Logic/Expression.h"
#include <typeinfo>
#include <iostream>
#include <cassert>

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

Expression List::Infer(const Knowledge& knowledge, const Substitutions& substitutions) const
{
    List result;
    std::transform(begin(), end(), std::back_inserter(result), 
        [&knowledge, &substitutions](const Expression& item)
        {
            return item.Infer(knowledge, substitutions);
        });
    return result;
}


Expression List::Simplify() const
{
    return List(SimplifyItems());
}


List List::Substitute(const Substitutions& substitutions) const
{
    return List(SubstituteItems(substitutions));
}

Expression List::Matches(const Expression& expression, const Substitutions& substitutions) const
{
    const List* list = expression.GetIf<List>();
    if (!list)
        return Boolean(false);
    auto substituted = list->Substitute(substitutions);

    return Collection::Matches(Collection_subrange(substituted.begin(), substituted.end()), substitutions);
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

