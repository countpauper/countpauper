#include "Logic/Integer.h"
#include "Logic/Internal/Number.h"
#include <string>
#include <stdexcept> 
#include <iostream>
#include <cmath>
#include <cassert>

namespace Angel::Logic
{

Integer::Integer(const Boolean& b) :
    value(bool(b))
{
}


Integer::Integer(const Real& r) :
    value(double(r))
{
}

Integer::Integer(const std::string_view s) :
    value(0)
{
    auto result = std::from_chars(s.begin(),s.end(),value,10);
    if (result.ptr != s.end())
       throw std::invalid_argument(std::format("Trailing character when converting {} to integer", s));
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

Integer::operator long() const
{
	return value;
}

bool Integer::operator==(const Integer& integer) const
{
	return value== integer.value;
}


bool Integer::operator<(const Integer& rhs) const
{
    return value < rhs.value;
}

Integer Integer::operator-() const
{
    return Integer(-value);
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


Number operator+(const Integer& lhs, const Integer& rhs) 
{ 
    return Integer(long(lhs) + long(rhs));
}
Number operator-(const Integer& lhs, const Integer& rhs) 
{ 
    return Integer(long(lhs) - long(rhs));
}

Number operator*(const Integer& lhs, const Integer& rhs) 
{ 
    return Integer(long(lhs) * long(rhs));
}
Number operator/(const Integer& lhs, const Integer& rhs) 
{ 
    return Integer(long(lhs) / long(rhs));
}

long IntegerPower(long base, long exponent)
{
    long result = 1;

    while (exponent > 0)
    {
        if (exponent % 2)
            result = result * base;
        base = base * base;
        exponent /= 2;
    }
    return result;
}

Number operator^(const Integer& lhs, const Integer& rhs) 
{ 
    long base = long(lhs);
    long exponent = long(rhs);

    if (exponent >= 0)
    {
        unsigned bits_needed = std::log2(std::abs(base)) * exponent;
        if (bits_needed <= sizeof(long)*8 -1)
        {
            return Integer(IntegerPower(base, exponent));
        }
    }
    return Number(std::pow(base, exponent));
}

Number log(const Integer& lhs, const Integer& rhs)
{
    long base = long(lhs);
    long value = long(rhs);

    assert(base>0);  // TODO: negative values require complex
    assert(value>0);  // TODO: negative values require complex
    assert(value!=1); // goes to infinity, which requires Integer
    double result = std::log10(base) / std::log10(value);
    assert(result<std::numeric_limits<long>::max());    // automatic conversion on overflow not yet implemented
    return Number(result);
}

}