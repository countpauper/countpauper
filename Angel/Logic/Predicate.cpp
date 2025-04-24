#include <algorithm>
#include "Logic/Predicate.h"
#include "Logic/Boolean.h"
#include "Logic/Knowledge.h"

namespace Angel::Logic
{

Predicate::Predicate(const Predicate& other) :
    Predicate(other.id, Sequence(other.arguments))
{

}

Predicate::Predicate(Predicate&& other) :
    id(other.id),
    arguments(std::move(other.arguments))
{
}

Predicate::Predicate(const Id& id, Sequence&& arguments) :
	id(id),
	arguments(std::move(arguments))
{
}

Predicate::Predicate(const std::string& tag, Sequence&& arguments) :
	id(tag),
	arguments(std::move(arguments))
{
}

Predicate::operator bool() const
{
    return true;
}

bool Predicate::operator==(const Expression& other) const
{
	if (auto predicate = dynamic_cast<const Predicate*>(&other))
	{
		return id == predicate->id && arguments == predicate->arguments;
	}
	return false;
}

std::size_t Predicate::Hash() const
{
    return id.Hash() ^ arguments.Hash();
}

std::ostream& operator<<(std::ostream& os, const Predicate& predicate)
{
	if (predicate.arguments)
	{
		os << predicate.id << predicate.arguments;
	}
	else 
	{
		os << predicate.id;
	}
    return os;
}

Object Predicate::Copy() const
{
    return Create<Predicate>(*this);
}

Match Predicate::Matching(const Expression& expr) const
{
	if (auto predicate = dynamic_cast<const Predicate*>(&expr))
	{
        if (id != predicate->id)
            return NoMatch;
		return arguments.Matching(predicate->arguments);
	}
	return NoMatch;
}

const Object* Predicate::Condition() const
{
	return nullptr;
}

Object Predicate::Infer(const Knowledge& known, const Variables& substitutions) const
{
    auto match = known.Match(*this);
    return match.Infer(known, substitutions);
}

Object Predicate::Cast(const std::type_info& t, const Knowledge& k) const
{
    if (typeid(t) == typeid(Boolean))
        return Infer(k, Variables());
    throw CastException<Predicate>(t);
}

Object predicate(const Id& id, Sequence&& arguments)
{
	return Create<Predicate>(id, std::move(arguments));
}

Object predicate(const std::string& name, Sequence&& arguments)
{
	return Create<Predicate>(Id(name), std::move(arguments));
}

}