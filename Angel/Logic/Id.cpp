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

bool Id::operator==(const Item& other) const
{
	if (auto id = dynamic_cast<const Id*>(&other))
	{
		return name == id->name;
	}
	return false;
}

Object id(const std::wstring& name)
{
	return Object(std::make_unique<Id>(name));
}


}
}