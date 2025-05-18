#include "Logic/Association.h"
#include "Logic/Expression.h"
#include "Logic/Object.h"

namespace Angel::Logic
{


class Expression;
class Object;

Association::Association(const Association& o) : 
    lhs(std::make_unique<Expression>(*o.lhs)),
    rhs(std::make_unique<Expression>(*o.rhs))
{
}

Association::Association(Association&& o) : 
    lhs(std::move(o.lhs)),
    rhs(std::move(o.rhs))
{
}

Association::Association(Expression&& lhs, Expression&& rhs) : 
    lhs(std::make_unique<Expression>(std::move(lhs))),
    rhs(std::make_unique<Expression>(std::move(rhs)))
{
}

Association& Association::operator=(const Association& o)
{
    lhs = std::make_unique<Expression>(*o.lhs);
    rhs = std::make_unique<Expression>(*o.rhs);
    return *this;
}

Association& Association::operator=(Association&& o)
{
    lhs.reset();
    rhs.reset();
    std::swap(lhs, o.lhs);
    std::swap(rhs, o.rhs);
    return *this;
}
Association::operator bool() const
{
    return bool(lhs);
}

std::size_t Association::size() const
{
    return static_cast<std::size_t>(bool(lhs)) + 
        static_cast<std::size_t>(bool(rhs));
}

const Expression& Association::Left() const
{
    return *lhs;
}

const Expression& Association::Right() const
{
    return *rhs;
}


Match Association::Matches(const Expression& expression, const Variables& vars) const
{
    // TODO what does it mean? does only lhs need to match? 
    return NoMatch;

}

Object Association::Infer(const class Knowledge& k, const Variables& substitutions) const
{
    return Association{
        std::move(*lhs),
        rhs->Infer(k, substitutions)
    };
}

Expression Association::Get(const Expression& key) const
{
    if (*lhs == key)
        return *rhs;
    else 
        return Boolean(false);
}

bool Association::operator==(const Association& other) const
{
    if (!lhs && bool(other.lhs))
        return false;
    if (!rhs && bool(other.rhs))
        return false;
    return Left() == other.Left() && 
        Right() == other.Right();
}

std::size_t Association::Hash() const
{
    std::hash<Expression> hasher;
    std::size_t result = 0;
    if (lhs)
        result ^= hasher(*lhs);
    if (rhs)
        result ^= hasher(*rhs);
    return result;
}

std::ostream& operator<<(std::ostream& os, const Association& a)
{
    os << a.Left() << ":" << a.Right();
    return os;
}

/*
Clause::Clause(Predicate&& predicate, Expression&& condition) :
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

const Expression* Clause::Condition() const
{
    return &condition;
}

Object Clause::Infer(const Knowledge& known, const Variables& substitutions) const
{
    return known.Query(condition, substitutions);
}

Expression Clause::Copy() const
{
    return clause(Predicate(predicate), Expression(std::move(condition->Copy())));
}


Expression Clause::Cast(const std::type_info& t, const Knowledge& k) const
{
    if (typeid(t) == typeid(Boolean))
        return Infer(k, Variables());
    throw CastException(typeid(*this), t);
}

Expression clause(Predicate&& predicate, Expression&& condition)
{
	return Create<Clause>(std::move(predicate), std::move(condition));
}
*/
}