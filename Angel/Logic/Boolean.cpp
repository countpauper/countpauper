#include "Logic/Boolean.h"
#include "Logic/Integer.h"
#include "Logic/CastException.h"
#include <format>
#include <stdexcept>
#include <iostream>

namespace Angel::Logic
{

Boolean::Boolean(bool v) :
	truth(v)
{
}

Boolean::Boolean(const std::string_view tag)
{
	if (tag == "true")
		truth = true;
	else if (tag == "false")
		truth = false;
	else
        throw std::invalid_argument(std::format("Invalid Boolean value `{}`", tag));
}


Boolean::operator bool() const 
{
    return truth;
}

std::size_t Boolean::Hash() const
{
    return typeid(decltype(*this)).hash_code() ^ truth;
}

bool Boolean::operator==(const Boolean& other) const
{
    return truth == other.truth;
}

bool Boolean::operator*() const
{
	return truth;
}


/*
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
*/

Boolean::operator std::string() const
{
    if (truth)
        return "true";
    else
        return "false";    
}

std::ostream& operator<<(std::ostream& os, const Boolean& element)
{
    os << std::string(element);
    return os;
}

/*
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
*/



}