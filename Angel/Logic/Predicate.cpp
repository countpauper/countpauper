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
bool Predicate::operator==(const Value& value) const
{
	if (auto predicate = dynamic_cast<const Predicate*>(&value))
	{
		return id == predicate->id && arguments == predicate->arguments;
	}
	return false;
}

bool Predicate::Match(const Value& value, const Knowledge& knowledge) const
{
	if (auto predicate = dynamic_cast<const Predicate*>(&value))
	{
		if (id != predicate->id)
			return false;
		return arguments.Match(predicate->arguments, knowledge);
	}
	return false;

}

void Predicate::Argue(Element&& value)
{
	arguments.Append(std::move(value));
}



Element predicate(const Id& id, Sequence&& arguments)
{
	return Element(std::make_unique<Predicate>(id, std::move(arguments)));
}

Element predicate(const std::wstring& name, Sequence&& arguments)
{
	return Element(std::make_unique<Predicate>(Id(name), std::move(arguments)));
}

}
}