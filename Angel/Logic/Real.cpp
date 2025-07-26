#include "Logic/Real.h"
#include "Logic/Internal/Number.h"
#include <string>
#include <stdexcept> 
#include <iostream>
#include <cmath>
#include <cassert>

namespace Angel::Logic
{

Real::Real(const Boolean& b) :
    value(bool(b))
{
}


Real::Real(const Integer& i) :
    value(long(i))
{
}

Real::Real(const std::string_view s) :
    value(0)
{
    auto result = std::from_chars(s.begin(),s.end(),value);
    if (result.ptr != s.end())
       throw std::invalid_argument(std::format("Trailing character when converting {} to real", s));
    if (result.ec == std::errc())
        return;
    else if (result.ec == std::errc::result_out_of_range)
        throw std::out_of_range(std::format("Value too large to convert {} to real", s));
    else
       throw std::invalid_argument(std::format("Failed to convert {} to real", s));
}

Real::operator bool() const
{
    return value!=0;
}

Real::operator double() const
{
	return value;
}

bool Real::operator==(const Real& integer) const
{
	return value== integer.value;
}

bool Real::operator<(const Real& rhs) const
{
    return value < rhs.value;
}

Real Real::operator-() const
{
    return Real(-value);
}

std::size_t Real::Hash() const
{
    return value;
}

std::ostream& operator<<(std::ostream& os, const Real& real)
{
    os << real.value;
    return os;
}
Number operator+(const Real& lhs, const Real& rhs) 
{ 
    return Number(double(lhs) + double(rhs));
}
Number operator-(const Real& lhs, const Real& rhs) 
{ 
    return Number(double(lhs) - double(rhs));
}

Number operator*(const Real& lhs, const Real& rhs) 
{ 
    return Number(double(lhs) * double(rhs));
}
Number operator/(const Real& lhs, const Real& rhs) 
{ 
    return Number(double(lhs) / double(rhs));
}
Number operator^(const Real& lhs, const Real& rhs) 
{ 
    return Number(std::pow(double(lhs), double(rhs)));
}

Number log(const Real& lhs, const Real& rhs)
{
    auto base = double(lhs);
    auto value = double(rhs);
    assert(base>0);  // TODO: negative values require complex
    assert(value>0);  // TODO: negative values require complex
    if (base == 10.0)
        return Real(std::log10(value));
    return Number(std::log(base) / std::log(value));
}
}