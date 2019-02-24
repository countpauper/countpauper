#include "stdafx.h"
#include "Boolean.h"
#include "Integer.h"

namespace Angel
{
namespace Logic
{

Boolean::Boolean(bool v) :
	truth(v)
{
}

bool Boolean::operator==(const Item& other) const
{
	if (auto boolean = dynamic_cast<const Boolean*>(&other))
	{
		return truth == boolean->truth;
	}
	else if (auto integer = dynamic_cast<const Integer*>(&other))
	{
		return truth == (**integer != 0);
	}
	return false;
}

bool Boolean::operator*() const
{
	return truth;
}


std::optional<bool> Boolean::Parse(const std::wstring& tag)
{
	if (tag == L"true")
		return true;
	else if (tag == L"false")
		return false;
	else
		return std::optional<bool>();
}

Object boolean(bool v)
{
	return Object(std::make_unique<Boolean>(v));
}


}
}