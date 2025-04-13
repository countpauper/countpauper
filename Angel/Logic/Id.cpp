#include "Logic/Id.h"
#include "Logic/Boolean.h"
#include <iostream>

namespace Angel
{
namespace Logic
{

Id::Id(const std::string& name) :
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

std::string Id::String() const
{
    return name;
}

Object Id::Compute(const Knowledge& known) const
{
    return id(name);
}

Object Id::Cast(const std::type_info& t, const Knowledge& k) const
{
    if (t == typeid(Id))
    {
        return Compute(k);
    }
    else if (t == typeid(Boolean))
    {
        return boolean(true);
    }
    throw CastException<Id>(t);
}


Object id(const std::string_view name)
{
	return Create<Id>(std::string(name));
}


}
}