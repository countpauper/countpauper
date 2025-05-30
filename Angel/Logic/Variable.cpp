#include "Logic/Variable.h"
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

Expression Variable::Substitute(const Hypothesis& hypothesis) const
{
    for(const auto& condition : hypothesis)
    {
        if (const auto* equation = condition.GetIf<Equation>())
        {
            if (equation->front() == *this)
            {   
                return equation->back().Substitute(hypothesis);
            }
            if (equation->back() == *this)
            {
                return equation->front().Substitute(hypothesis);
            }
        }
    }
    return *this;
}

Expression Variable::Matches(const Expression& e, const Hypothesis& hypothesis, bool reverse) const
{
    auto substitute = Substitute(hypothesis);
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
        return substitute.Matches(e, hypothesis);
    }
}


Expression Variable::Infer(const class Knowledge& k, Hypothesis& hypothesis) const
{
    for(const auto& condition : hypothesis)
    {
        if (const auto* equation = condition.GetIf<Equation>())
        {
            // not determined what to do with long equations or unequations 
            // if any item in the equation is the variable another can be inferred.
            // this could have a preference: elements highest, oprations middle, variables last
            assert(equation->size()==2); 
            if (equation->front() == *this)
            {
                return equation->back().Infer(k, hypothesis);
            }
            if (equation->back() == *this)
            {
                return equation->front().Infer(k, hypothesis);
            }
        }
    }
    return *this;
}



std::ostream& operator<<(std::ostream& os, const Variable& id)
{
    os << "$" << id.Name();
    return os;
}

}

