#include "Logic/Clause.h"
#include "Logic/Knowledge.h"

namespace Angel
{
namespace Logic
{

Clause::Clause(Predicate&& predicate, Object&& condition) :
	predicate(std::move(predicate)),
	condition(std::move(condition))
{
}

Clause::operator bool() const
{
    return bool(predicate);
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

std::size_t Clause::Hash() const
{
    return predicate.Hash() ^ condition.Hash();
}

std::ostream& operator<<(std::ostream& os, const Clause& clause)
{
    const auto* trivalCondition = clause.condition.As<Boolean>();
    if (trivalCondition && *trivalCondition)
    {
        os << clause.predicate;
    }
    else 
    {
        os << clause.predicate << " : " << clause.condition;
    }
    return os;
}


Match Clause::Matching(const Expression& other, const Variables& substitutions) const
{
	if (auto query = dynamic_cast<const Predicate*>(&other))
	{
        return predicate.Matching(*query, substitutions);
    }
    else if (auto clause = dynamic_cast<const Clause*>(&other))
    {
        return predicate.Matching(clause->predicate, substitutions);
    }
    return NoMatch;
}

const Object* Clause::Condition() const
{
    return &condition;
}

Object Clause::Infer(const Knowledge& known, const Variables& substitutions) const
{
    return known.Query(condition, substitutions);
}

Object Clause::Copy() const
{
    return clause(Predicate(predicate), Object(std::move(condition->Copy())));
}


Object Clause::Cast(const std::type_info& t, const Knowledge& k) const
{
    if (typeid(t) == typeid(Boolean))
        return Infer(k, Variables());
    throw CastException<Clause>(t);
}

Object clause(Predicate&& predicate, Object&& condition)
{
	return Create<Clause>(std::move(predicate), std::move(condition));
}

}
}