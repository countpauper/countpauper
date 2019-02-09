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

bool Predicate::operator==(const Value& other) const
{
	if (auto predicate = dynamic_cast<const Predicate*>(&other))
	{
		return id == predicate->id;
	}
	return false;
}



}
}