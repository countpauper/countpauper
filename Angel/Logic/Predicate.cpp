#include "stdafx.h"
#include "Predicate.h"

namespace Angel
{
namespace Logic
{

Predicate::Predicate(const Id& id) :
	id(id)
{
}

Predicate::Predicate(const std::wstring& name) :
	id(name)
{
}

bool Predicate::operator==(const Value& value) const
{
	if (auto predicate = dynamic_cast<const Predicate*>(&value))
	{
		return id == predicate->id;
	}
	return false;
}

Element predicate(const std::wstring& name, Element&& sequence)
{
	return Element(std::make_unique<Predicate>(name));
}


}
}