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

Object Predicate::Match(const Expression& expr) const
{
	if (auto predicate = dynamic_cast<const Predicate*>(&expr))
	{
        if (id != predicate->id)
            return boolean(false);;
		return arguments.Match(predicate->arguments);
	}
	return boolean(false);
}

Object Predicate::Compute(const Knowledge& known) const
{
    auto match = known.Match(*this);
    return match.Compute(known);
}

Object Predicate::Cast(const std::type_info& t, const Knowledge& k) const
{
    if (typeid(t) == typeid(Boolean))
        return Compute(k);
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