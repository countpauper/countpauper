#include <algorithm>
#include "Logic/Predicate.h"
#include "Logic/Boolean.h"
#include "Logic/Knowledge.h"
#include "Logic/Expression.h"

namespace Angel::Logic
{

Predicate::Predicate(Predicate&& o) :
	id()
{
	std::swap(id, o.id);
	std::swap(arguments, o.arguments);
}

Predicate& Predicate::operator=(const Predicate& o)
{
	id = o.id;
	arguments = o.arguments;
	return *this;
}

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

Predicate::operator bool() const
{
    return bool(id);
}

bool Predicate::operator==(const Predicate& rhs) const
{
	return id == rhs.id && arguments == rhs.arguments;
}

std::size_t Predicate::Hash() const
{
    return id.Hash() ^ arguments.Hash();
}

Match Predicate::Matches(const Expression& inferred, const Variables& vars) const
{
	const auto* predicate = std::get_if<Predicate>(&inferred);
	if (!predicate)
		return NoMatch;
	
	if (id != predicate->id) // Variable predicate names not (yet) supported
		return NoMatch;
	return arguments.Matches(predicate->arguments, vars);
}

Object Predicate::Infer(const Knowledge& knowledge, const Variables& substitutions) const
{
	Predicate computed(id, std::get<List>(arguments.Infer(knowledge, substitutions)));
	auto matches = knowledge.Matches(computed);
	for(const auto& association: matches)
	{
		if (association.second == Boolean(false))
			continue;
		return association.second.Infer(knowledge, substitutions);
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