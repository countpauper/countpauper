#include "stdafx.h"
#include <algorithm>
#include "Predicate.h"

namespace Angel
{
namespace Logic
{

Predicate::Predicate(const Id& id, Sequence&& arguments) :
	id(id),
	arguments(std::move(arguments))
{
}

Predicate::Predicate(const std::wstring& tag, Sequence&& arguments) :
	id(tag),
	arguments(std::move(arguments))
{
}
bool Predicate::operator==(const Item& other) const
{
	if (auto predicate = dynamic_cast<const Predicate*>(&other))
	{
		return id == predicate->id && arguments == predicate->arguments;
	}
	return false;
}

bool Predicate::Match(const Item& item, const Knowledge& knowledge) const
{
	if (auto predicate = dynamic_cast<const Predicate*>(&item))
	{
		if (id != predicate->id)
			return false;
		return arguments.Match(predicate->arguments, knowledge);
	}
	return false;

}

void Predicate::Argue(Object&& value)
{
	arguments.Append(std::move(value));
}



Object predicate(const Id& id, Sequence&& arguments)
{
	return Object(std::make_unique<Predicate>(id, std::move(arguments)));
}

Object predicate(const std::wstring& name, Sequence&& arguments)
{
	return Object(std::make_unique<Predicate>(Id(name), std::move(arguments)));
}

}
}