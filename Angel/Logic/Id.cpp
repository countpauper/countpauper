#include "Logic/Id.h"
#include "Logic/Boolean.h"
#include <iostream>
#include <unordered_map>

namespace Angel::Logic
{

Id::Id(const std::string_view name) :
	name(name)
{
}

Id::operator bool() const
{
    return !name.empty();
}

bool Id::operator==(const Id& id) const
{
	return name == id.name;
}

std::size_t Id::Hash() const
{
    std::hash<std::string> hasher;
    return hasher(name);
}

/*

Object Id::Cast(const std::type_info& t, const Knowledge& k) const
{
    if (t == typeid(Id))
    {
        return id(name);
    }
    else if (t == typeid(Boolean))
    {
        return boolean(true);
    }
    throw CastException<Id>(t);
}
*/

std::ostream& operator<<(std::ostream& os, const Id& id)
{
    os << id.name.c_str();
    return os;
}

/*
Object id(const std::string_view name)
{
	return Create<Id>(std::string(name));
}
*/

}