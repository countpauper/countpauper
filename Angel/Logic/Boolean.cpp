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


std::optional<bool> Boolean::Parse(const std::string& tag)
{
	if (tag == "true")
		return true;
	else if (tag == "false")
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

Object boolean(const std::string_view v)
{
    if (v == "true")
        return boolean(true);
    else if (v == "false")
        return boolean(false);
    else
        throw std::invalid_argument(std::string("Unrecognized boolean value:") + std::string(v));
}


}