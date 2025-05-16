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

Integer::Integer(const std::string_view s) :
    value(0)
{
    auto result = std::from_chars(s.begin(),s.end(),value,10);
    if (result.ptr != s.end())
       throw std::invalid_argument(std::format("Trailing character when converintg {} to integer", s));
    if (result.ec == std::errc())
        return;
    else if (result.ec == std::errc::result_out_of_range)
        throw std::out_of_range(std::format("Value too large to convert {} to integer", s));
    else
       throw std::invalid_argument(std::format("Failed to convert {} to integer", s));
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