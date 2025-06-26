#include "Logic/Boolean.h"
#include "Logic/CastException.h"
#include "Logic/Expression.h"
#include <format>
#include <stdexcept>
#include <iostream>

namespace Angel::Logic
{
    
Boolean::Boolean(const std::string_view tag)
{
	if (tag == "true")
		truth = true;
	else if (tag == "false")
		truth = false;
	else
        throw std::invalid_argument(std::format("Invalid Boolean value `{}`", tag));
}

Boolean::Boolean(const Integer& i) :
    Boolean(*i != 0)
{
}

Boolean::Boolean(const Collection& c) :
    Boolean(!c.empty())
{
}

Boolean::Boolean(const Set& s) :
    Boolean(s.size()>0)
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

bool Boolean::operator==(const Boolean& other) const
{
    return truth == other.truth;
}

bool Boolean::operator*() const
{
	return truth;
}

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
Expression boolean(const std::string_view v)
{
    if (v == "true")
        return boolean(true);
    else if (v == "false")
        return boolean(false);
    else
        throw std::invalid_argument(std::string("Unrecognized boolean value:") + std::string(v));
}

Expression boolean(bool v)
{
	return Create<Boolean>(v);
}
*/



}