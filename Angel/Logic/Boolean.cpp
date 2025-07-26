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
    Boolean(long(i) != 0)
{
}


Boolean::Boolean(const Real& r) :
    Boolean(double(r)!=0)
{
}

Boolean::Boolean(const Tuple& c) :
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

Boolean& Boolean::operator&=(const Boolean& rhs)
{
    truth = truth && bool(rhs);
    return *this;
}

Boolean& Boolean::operator|=(const Boolean& rhs) 
{
    truth = truth || bool(rhs);
    return *this;
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

Expression operator&(Boolean lhs, const Expression& rhs)
{
    if (lhs == True)
        return rhs; 
    else 
        return lhs;
}

Expression operator|(Boolean lhs, const Expression& rhs)
{
    if (lhs == False)
        return rhs;
    else 
        return lhs;
}


}