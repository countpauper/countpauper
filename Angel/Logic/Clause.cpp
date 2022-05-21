#include "stdafx.h"
#include "Clause.h"
#include "Knowledge.h"

namespace Angel
{
namespace Logic
{

Clause::Clause(Predicate&& predicate, Object&& condition) :
	predicate(std::move(predicate)),
	condition(std::move(condition))
{
}


bool Clause::operator==(const Expression& value) const
{
	if (auto clause = dynamic_cast<const Clause*>(&value))
	{
		return predicate == clause->predicate && 
			condition == clause->condition;
	}
	return false;
}

Object Clause::Match(const Expression& value) const
{
	if (auto query = dynamic_cast<const Predicate*>(&value))
	{
        auto predicateMatch = predicate.Match(*query);
        // TODO: non trivial predicate match with variable argumentss
        if (predicateMatch.Trivial())
        {
            // Can't just query it, there won't be a single matching conjunction.
            // Need to Compute it (or =cast it to boolean) which would then require iterating
            // TODO: might just want to copy it and lazy compute it later? Is that lazy or slow? 
            return Object(condition);
        }
        else
            return predicateMatch;
	}
    else if (auto clause = dynamic_cast<const Clause*>(&value))
    {
        auto predicateMatch = predicate.Match(clause->predicate);
        if (predicateMatch.Trivial())
        {
            return condition->Match(*(clause->condition));
        }
    }
	return boolean(false);
}

Object Clause::Compute(const Knowledge& known) const
{
    assert(false); // Compute the expression and then return the predicate? What if the expression is false?  
    return Object();
}


Object Clause::Copy() const
{
    return clause(Predicate(predicate), Object(std::move(condition->Copy())));
}


Object Clause::Cast(const std::type_info& t, const Knowledge& k) const
{
    if (typeid(t) == typeid(Boolean))
        return Compute(k);
    throw CastException<Clause>(t);
}

Object clause(Predicate&& predicate, Object&& condition)
{
	return Create<Clause>(std::move(predicate), std::move(condition));
}

}
}