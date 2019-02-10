#include "stdafx.h"
#include "Id.h"

namespace Angel
{
namespace Logic
{

Id::Id(const std::wstring& name) :
	name(name)
{
}

bool Id::operator==(const Id& id) const
{
	return name == id.name;
}

bool Id::operator==(const Value& value) const
{
	if (auto id = dynamic_cast<const Id*>(&value))
	{
		return name == id->name;
	}
	return false;
}

Element id(const std::wstring& name)
{
	return Element(std::make_unique<Id>(name));
}


}
}