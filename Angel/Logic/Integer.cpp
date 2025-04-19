#include "Logic/Integer.h"
#include "Logic/Boolean.h"

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

bool Integer::operator==(const Expression& other) const
{
	if (auto integer= dynamic_cast<const Integer*>(&other))
	{
		return operator==(*integer);
	}
	return false;
}

std::string Integer::String() const
{
    return std::to_string(value);
}

long Integer::operator*() const
{
	return value;
}

Object Integer::Compute(const Knowledge& known) const
{
    return integer(value);
}

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

std::optional<long> Integer::Parse(const std::string& tag)
{
	size_t parsedLength=0;
	long result = std::stol(tag.c_str(), &parsedLength, 10);
	if ((parsedLength==tag.size()) && (parsedLength))
	{
		return result;
	}
	else
	{
		return std::optional<long>();
	}
}



}
}