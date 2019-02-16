#include "stdafx.h"
#include <algorithm>
#include "Predicate.h"

namespace Angel
{
namespace Logic
{

Predicate::Predicate(const Id& id, Sequence&& parameters) :
	id(id),
	parameters(std::move(parameters))
{
}

Predicate::Predicate(const std::wstring& tag, Sequence&& parameters) :
	id(tag),
	parameters(std::move(parameters))
{
}
bool Predicate::operator==(const Value& value) const
{
	if (auto predicate = dynamic_cast<const Predicate*>(&value))
	{
		return id == predicate->id && parameters == predicate->parameters;
	}
	return false;
}

bool Predicate::Match(const Value& value, const Knowledge& knowledge) const
{
	if (auto predicate = dynamic_cast<const Predicate*>(&value))
	{
		if (id != predicate->id)
			return false;
		return parameters.Match(predicate->parameters, knowledge);
	}
	return false;

}


Element predicate(const Id& id, Sequence&& sequence)
{
	return Element(std::make_unique<Predicate>(id, std::move(sequence)));
}

Element predicate(const std::wstring& name, Sequence&& sequence)
{
	return Element(std::make_unique<Predicate>(Id(name), std::move(sequence)));
}

Element predicate(const std::wstring& name)
{
	return Element(std::make_unique<Predicate>(name));
}

}
}