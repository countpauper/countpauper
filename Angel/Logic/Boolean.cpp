#include "stdafx.h"
#include "Boolean.h"
#include "Integer.h"

namespace Angel::Logic
{

Boolean::Boolean(bool v) :
	truth(v)
{
}

bool Boolean::operator==(const Expression& other) const
{
	if (auto boolean = dynamic_cast<const Boolean*>(&other))
	{
		return truth == boolean->truth;
	}
    // TODO: else cast to Boolean, then do ti
	else if (auto integer = dynamic_cast<const Integer*>(&other))
	{
		return truth == (**integer != 0);
	}
	return false;
}

Object Boolean::Compute(const Knowledge& known) const
{
    return boolean(truth);
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


Object Boolean::Cast(const std::type_info& t, const Knowledge& k) const
{
    if (t == typeid(Boolean))
    {
        return boolean(truth);
    }
    else if (t == typeid(Integer))
    {
        return integer(truth ? 1 : 0);
    }
    throw CastException<Boolean>(t);
}

Object boolean(bool v)
{
	return Create<Boolean>(v);
}


}