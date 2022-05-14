#include "stdafx.h"
#include "Id.h"
#include "Boolean.h"

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

bool Id::operator==(const Expression& other) const
{
	if (auto id = dynamic_cast<const Id*>(&other))
	{
		return name == id->name;
	}
	return false;
}

Object Id::Compute(const Knowledge& known) const
{
    return id(name);
}

Object Id::Cast(const std::type_info& t, const Knowledge& k) const
{
    if (t == typeid(Boolean))
    {
        return boolean(true);
    }
    throw CastException<Id>(t);
}


Object id(const std::wstring& name)
{
	return Create<Id>(name);
}


}
}