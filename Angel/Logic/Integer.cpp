#include "Logic/Integer.h"
#include "Logic/Boolean.h"
#include <string>
#include <stdexcept> 
#include <iostream>

namespace Angel
{
namespace Logic
{

Integer::Integer(long value) :
	value(value)
{
}

Integer::Integer(const Boolean& b) :
    value(*b)
{
}

Integer::operator bool() const
{
    return value!=0;
}

bool Integer::operator==(const Integer& integer) const
{
	return value== integer.value;
}


std::size_t Integer::Hash() const
{
    return value;
}

std::ostream& operator<<(std::ostream& os, const Integer& integer)
{
    os << integer.value;
    return os;
}

long Integer::operator*() const
{
	return value;
}

}
}