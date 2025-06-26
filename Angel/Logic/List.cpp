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


List& List::operator+=(const List& rhs)
{
    Collection::operator+=(rhs);
    return *this;
}
List& List::operator-=(const List& rhs)
{
    Collection::operator-=(rhs);
    return *this;
}
List& List::operator&=(const List& rhs)
{
    Collection::operator&=(rhs);
    return *this;
}
List& List::operator|=(const List& rhs)
{
    Collection::operator|=(rhs);
    return *this;
}

List operator+(List lhs, const List& rhs) 
{ 
    return lhs += rhs; 
}

List operator-(List lhs, const List& rhs)
{
    return lhs -= rhs;
}

List operator&(List lhs, const List& rhs)
{
    return lhs &= rhs;
}

List operator|(List lhs, const List& rhs)
{
    return lhs |= rhs;
}


Expression List::Infer(const Knowledge& knowledge, const Hypothesis& hypothesis, Trace& trace) const
{
    List result;
    std::transform(begin(), end(), std::back_inserter(result), 
        [&knowledge, &hypothesis, &trace](const Expression& item)
        {
            return item.Infer(knowledge, hypothesis, trace);
        });
    return result;
}


List List::Simplify() const
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
        return False;
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

