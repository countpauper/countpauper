#include "Logic/Integer.h"
#include "Logic/Boolean.h"
#include <string>
#include <stdexcept> 
#include <iostream>

namespace Angel
{
namespace Logic
{

Integer::Integer(long value) :
	value(value)
{
}

Integer::operator bool() const
{
    return value!=0;
}

bool Integer::operator==(const Integer& integer) const
{
	return value== integer.value;
}


std::size_t Integer::Hash() const
{
    return value;
}

std::ostream& operator<<(std::ostream& os, const Integer& integer)
{
    os << integer.value;
    return os;
}

long Integer::operator*() const
{
	return value;
}

/*
Object Integer::Cast(const std::type_info& t, const Knowledge& k) const
{
    if (t == typeid(Boolean))
    {
        return boolean(value != 0);
    }
    else if (t == typeid(Integer))
    {
        return integer(value);
    }
    throw CastException<Integer>(t);
}

Object integer(long value)
{
	return Create<Integer>(value);
}

Object integer(const std::string_view value)
{
    return Create<Integer>(std::stol(std::string(value)));
}

Integer Integer::Parse(const std::string& tag)
{
	size_t parsedLength=0;
	long result = std::stol(tag.c_str(), &parsedLength, 10);
	if ((parsedLength==tag.size()) && (parsedLength))
	{
		return Integer(result);
	}
	else
	{
		throw std::invalid_argument(std::format("Invalid integer value `{}`", tag))
	}
}

*/


}
}