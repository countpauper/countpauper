#include <algorithm>
#include "Logic/Predicate.h"
#include "Logic/Boolean.h"
#include "Logic/Knowledge.h"
#include "Logic/Object.h"

namespace Angel::Logic
{


Predicate::Predicate(const Id& id, List&& arguments) 
	: id(id)
	, arguments(std::move(arguments))
{
}

Predicate::Predicate(const std::string& tag, List&& arguments) 
	: Predicate(Id(tag), std::move(arguments))
{
}

Predicate::Predicate(const Object& o) :
	Predicate(o.Cast<Predicate>())
{
}

Predicate::operator bool() const
{
    return true;
}

bool Predicate::operator==(const Predicate& rhs) const
{
	return id == rhs.id && arguments == rhs.arguments;
}


std::size_t Predicate::Hash() const
{
    return id.Hash() ^ arguments.Hash();
}


Match Predicate::Matches(const Predicate& predicate, const Variables& substitutions) const
{
	if (id != predicate.id) // Variable predicate names not (yet) supported
		return NoMatch;
	return arguments.Matches(predicate.arguments, substitutions);
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

/*

Object Predicate::Copy() const
{
    return Create<Predicate>(*this);
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
*/

}