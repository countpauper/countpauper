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
	wchar_t* end=nullptr;
	long result = std::wcstol(tag.c_str(), &end,10);
	if ((*end == L'\x0') && (end!=tag.c_str()))
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