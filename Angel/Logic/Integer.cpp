#include "stdafx.h"
#include "Integer.h"
#include "Boolean.h"

namespace Angel
{
namespace Logic
{

Integer::Integer(long value) :
	value(value)
{
}

bool Integer::operator==(const Integer& integer) const
{
	return value== integer.value;
}

bool Integer::operator==(const Item& other) const
{
	if (auto integer= dynamic_cast<const Integer*>(&other))
	{
		return operator==(*integer);
	}
	else if (auto boolean = dynamic_cast<const Boolean*>(&other))
	{
		return **boolean == (value != 0);
	}
	return false;
}

long Integer::operator*() const
{
	return value;
}

Object integer(long value)
{
	return Object(std::make_unique<Integer>(value));
}

std::optional<long> Integer::Parse(const std::wstring& tag)
{
	std::size_t processed(0);
	long result(0);
	try
	{
		result = std::stol(tag, &processed);
	}
	catch(std::invalid_argument&)
	{
		return std::optional<long>();
	}
	if (processed != tag.size())
	{
		return std::optional<long>();
	}
	else
	{
		return result;
	}
}



}
}