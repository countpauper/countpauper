#include "stdafx.h"
#include "Clause.h"
#include "Knowledge.h"

namespace Angel
{
namespace Logic
{

Clause::Clause(Predicate&& predicate, Sequence&& conditions) :
	predicate(std::move(predicate)),
	conditions(std::move(conditions))
{
}

bool Clause::operator==(const Value& value) const
{
	if (auto clause = dynamic_cast<const Clause*>(&value))
	{
		return predicate == clause->predicate && 
			conditions == clause->conditions;
	}
	return false;
}

bool Clause::Match(const Value& value, const Knowledge& knowledge) const
{
	if (auto query = dynamic_cast<const Predicate*>(&value))
	{
		if (this->predicate == *query)
		{
			for (const auto& condition : conditions)
			{
				if (!knowledge.Query(condition))
					return false;
			}
		return true;
		}
	}
	else if (auto clause = dynamic_cast<const Clause*>(&value))
	{
		return this->predicate == clause->predicate;
	}
	return false;

}

Element clause(Predicate&& predicate, Sequence&& sequence)
{
	return Element(std::make_unique<Clause>(std::move(predicate), std::move(sequence)));
}

}
}