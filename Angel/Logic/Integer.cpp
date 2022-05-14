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

bool Integer::operator==(const Expression& other) const
{
	if (auto integer= dynamic_cast<const Integer*>(&other))
	{
		return operator==(*integer);
	}
    // standard, same type ==, else cast to my type, then check that expr ?

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