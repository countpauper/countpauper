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
	if (!id)
		throw std::invalid_argument("Predicate identity can not be empty.");
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

Match Predicate::Matches(const Predicate& predicate) const
{
	if (id != predicate.id) // Variable predicate names not (yet) supported
		return NoMatch;
	return arguments.Matches(predicate.arguments);
}

Object Predicate::Compute(const Knowledge& knowledge, const Variables& substitutions) const
{
	Predicate computed(id, std::get<List>(arguments.Compute(knowledge, substitutions)));
	auto matches = knowledge.Matches(computed);
	for(const auto& association: matches)
	{
		if (association.second == Boolean(false))
			continue;
		return association.second;
	}
	return Boolean(false);
}

std::ostream& operator<<(std::ostream& os, const Predicate& predicate)
{
	if (predicate.arguments)
	{
		os << predicate.id << "(";
		bool first = true;
		for(const auto& arg: predicate.arguments)
		{
			if (!first)
				os << ",";
			os << arg;
			first = false;
		}
		os << ")";
	}
	else
	{
		os << predicate.id;
	}
    return os;
}

}