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

Expression List::Infer(const Knowledge& knowledge, Hypothesis& hypothesis) const
{
    List result;
    std::transform(begin(), end(), std::back_inserter(result), 
        [&knowledge, &hypothesis](const Expression& item)
        {
            return item.Infer(knowledge, hypothesis);
        });
    return result;
}


Expression List::Simplify() const
{
    return List(SimplifyItems());
}


List List::Substitute(const Hypothesis& hypothesis) const
{
    return List(SubstituteItems(hypothesis));
}

Expression List::Matches(const Expression& expression, const Hypothesis& hypothesis) const
{
    const List* list = expression.GetIf<List>();
    if (!list)
        return Boolean(false);
    auto substituted = list->Substitute(hypothesis);

    return Collection::Matches(Collection_subrange(substituted.begin(), substituted.end()), hypothesis);
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

