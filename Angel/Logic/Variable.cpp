#include "Logic/Variable.h"
#include "Logic/Boolean.h"
#include "Logic/Expression.h"
#include <iostream>
#include <cassert>

namespace Angel::Logic
{

Variable::Variable(const std::string_view name) :
	name(name)
{
}

Variable::operator bool() const
{
    return !name.empty();
}

bool Variable::operator==(const Variable& var) const
{
	return name == var.name;
}

std::size_t Variable::Hash() const
{
    std::hash<std::string> hasher;
    return hasher(name);
}

std::string_view Variable::Name() const
{
    return name;
}

Expression Variable::Simplify() const
{
    return *this;   
}

Expression Variable::Substitute(const Variables& substitutions) const
{
    for(const auto& condition : substitutions)
    {
        if (const auto* equation = condition.GetIf<Equation>())
        {
            if (equation->front() == *this)
            {   
                return equation->back();
            }
            if (equation->back() == *this)
            {
                return equation->front();
            }
        }
    }
    return *this;
}

Match Variable::Matches(const Expression& e, const Variables& variables, bool reverse) const
{
    auto substitute = Substitute(variables);
    if (const auto* stillVar = substitute.GetIf<Variable>())
    {
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
        return substitute.Matches(e, variables);
    }
}


Expression Variable::Infer(const class Knowledge& k, const Variables& substitutions) const
{
    for(const auto& condition : substitutions)
    {
        if (const auto* equation = condition.GetIf<Equation>())
        {
            assert(equation->size()==2); // not determined what to do with long equations or unequations 
            if (equation->front() == *this)
            {
                return equation->back();
            }
            if (equation->back() == *this)
            {
                return equation->front();
            }
        }
    }
    return *this;
}



std::ostream& operator<<(std::ostream& os, const Variable& id)
{
    os << "$" << id.name.c_str();
    return os;
}

}

