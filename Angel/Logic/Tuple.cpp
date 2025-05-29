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

Expression Tuple::Simplify() const
{
    return *this;   
}

Expression Tuple::Substitute(const Substitutions& substitutions) const
{
    for(const auto& condition : substitutions)
    {
        if (const auto* equation = condition.GetIf<Equation>())
        {
            if (equation->front() == *this)
            {   
                return equation->back().Substitute(substitutions);
            }
            if (equation->back() == *this)
            {
                return equation->front().Substitute(substitutions);
            }
        }
    }
    return *this;
}

Expression Tuple::Matches(Collection_subrange range, const Substitutions& substitutions, bool reverse) const
{
    auto substitute = Substitute(substitutions);
    if (const auto* stillVar = substitute.GetIf<Tuple>())
    {
        if (*stillVar)
            if (reverse)
                return Equation{*stillVar, List(range)};
            else 
                return Equation{List(range), *stillVar};
        else
            return Boolean(true);   // anonymous tuple 
    }
    else 
    {
        if (const List* list = substitute.GetIf<List>())
        {
            return list->Collection::Matches(range, substitutions);
        }
        else 
        {
            assert(false); // unimplemented;
            return Boolean(false);
        }
    }  
}

Expression Tuple::Matches(const Expression& e, const Substitutions& substitutions, bool reverse) const
{
    auto substitute = Substitute(substitutions);
    if (const auto* stillVar = substitute.GetIf<Tuple>())
    {
        return Boolean(false);
        if (*stillVar)
            if (reverse)
                return Equation{*stillVar, e};
            else
                return Equation{e, *stillVar};
        else
            return Boolean(true);   // anonymous
    }
    else 
    {
        return substitute.Matches(e, substitutions);
    }
}


Expression Tuple::Infer(const class Knowledge& k, const Substitutions& substitutions) const
{
    for(const auto& condition : substitutions)
    {
        if (const auto* equation = condition.GetIf<Equation>())
        {
            assert(equation->size()==2); // not determined what to do with long equations or unequations 
            if (equation->front() == *this)
            {
                return equation->back().Infer(k, substitutions);
            }
            if (equation->back() == *this)
            {
                return equation->front().Infer(k, substitutions);
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

