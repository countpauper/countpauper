#include "Logic/Boolean.h"
#include "Logic/Integer.h"
#include "Logic/CastException.h"

namespace Angel::Logic
{

Boolean::Boolean(bool v) :
	truth(v)
{
}

Boolean::operator bool() const 
{
    return truth;
}

std::size_t Boolean::Hash() const
{
    return truth;
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

std::ostream& operator<<(std::ostream& os, const Boolean& element)
{
    if (element.truth)
        os << "true";
    else
        os << "false";
    return os;
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

Object boolean(bool v)
{
	return Create<Boolean>(v);
}



}