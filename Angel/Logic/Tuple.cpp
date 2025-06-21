#include "Logic/Tuple.h"
#include "Logic/Expression.h"
#include <iostream>
#include <cassert>

namespace Angel::Logic
{

Tuple::Tuple(const std::string_view name) :
	name(name)
{
}

Tuple::operator bool() const
{
    return !name.empty();
}

bool Tuple::operator==(const Tuple& var) const
{
	return name == var.name;
}

std::size_t Tuple::Hash() const
{
    std::hash<std::string> hasher;
    return hasher(name);
}

std::string_view Tuple::Name() const
{
    return name;
}

Tuple Tuple::Simplify() const
{
    return *this;   
}

Expression Tuple::Substitute(const Hypothesis& hypothesis) const
{
    for(const auto& condition : hypothesis)
    {
        if (const auto* equation = condition.GetIf<Equal>())
        {
            assert(equation->size() == 2); // not yet implemented multi equation or error for single equation
            if (equation->front() == Variable(name))
            {   
                return equation->back().Substitute(hypothesis);
            }
            if (equation->back() == Variable(name))
            {
                return equation->front().Substitute(hypothesis);
            }
        }
    }
    return *this;
}

Expression Tuple::Matches(Collection_subrange range, const Hypothesis& hypothesis, bool reverse) const
{
    auto substitute = Substitute(hypothesis);
    if (const auto* stillVar = substitute.GetIf<Tuple>())
    {
        if (*stillVar)
            if (reverse)
                return Equal{Variable(name), List(range)};
            else 
                return Equal{List(range), Variable(name)};
        else
            return Boolean(true);   // anonymous tuple 
    }
    else 
    {
        if (const List* list = substitute.GetIf<List>())
        {
            return list->Collection::Matches(range, hypothesis);
        }
        else 
        {
            assert(false); // unimplemented;
            return Boolean(false);
        }
    }  
}

Expression Tuple::Matches(const Expression& e, const Hypothesis& hypothesis, bool reverse) const
{
    auto substitute = Substitute(hypothesis);
    if (const auto* stillVar = substitute.GetIf<Tuple>())
    {
        if (*stillVar)
            if (reverse)
                return Equal{Variable(name), e};
            else
                return Equal{e, Variable(name)};
        else
            return Boolean(true);   // anonymous
    }
    else 
    {
        return substitute.Matches(e, hypothesis);
    }
}


Expression Tuple::Infer(const class Knowledge& k, const Hypothesis& hypothesis, Trace& trace) const
{
    for(const auto& condition : hypothesis)
    {
        if (const auto* equation = condition.GetIf<Equal>())
        {
            assert(equation->size()==2); // not determined what to do with long equations or unequations 
            if (equation->front() == Variable(name))
            {
                return equation->back().Infer(k, hypothesis, trace);
            }
            if (equation->back() == Variable(name))
            {
                return equation->front().Infer(k, hypothesis, trace);
            }
        }
    }
    return *this;
}



std::ostream& operator<<(std::ostream& os, const Tuple& id)
{
    os << "*" << id.Name();
    return os;
}

}

