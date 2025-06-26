#include "Logic/Variable.h"
#include "Logic/Expression.h"
#include <iostream>
#include <cassert>

namespace Angel::Logic
{

Variable::Variable(const std::string_view name) :
	Individual(name.empty()?Expression():Id(name))
{
}

bool Variable::operator==(const Variable& var) const
{
	return Individual::operator==(var);
}

std::size_t Variable::Hash() const
{
    return Individual::Hash() ^ typeid(*this).hash_code();
}

Variable Variable::Simplify() const
{
    return *this;   
}

Expression Variable::Substitute(const Hypothesis& hypothesis) const
{
    if (empty())
        return *this;   // anonymous

    const auto* id = (*this)->Substitute(hypothesis).GetIf<Id>();
    if (!id)
        return *this;   // variable variable 

    for(const auto& condition : hypothesis)
    {
        if (const auto* equation = condition.GetIf<Equal>())
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
        if (stillVar->empty())
            return True;   // anonymous
        if (reverse)
            return Equal{*stillVar, e};
        else
            return Equal{e, *stillVar};
    }
    else 
    {
        return substitute.Matches(e, hypothesis);
    }
}


Expression Variable::Infer(const class Knowledge& k, const Hypothesis& hypothesis, Trace& trace) const
{
    for(const auto& condition : hypothesis)
    {
        if (const auto* equation = condition.GetIf<Equal>())
        {
            // not determined what to do with long equations or unequations 
            // if any item in the equation is the variable another can be inferred.
            // this could have a preference: elements highest, oprations middle, variables last
            assert(equation->size()==2); 
            if (equation->front() == *this)
            {
                return equation->back().Infer(k, hypothesis, trace);
            }
            if (equation->back() == *this)
            {
                return equation->front().Infer(k, hypothesis, trace);
            }
        }
    }
    return *this;
}

std::ostream& operator<<(std::ostream& os, const Variable& var)
{
    os << var.ope;
    if (var)
        os << to_string(*var);
    return os;
}

}

