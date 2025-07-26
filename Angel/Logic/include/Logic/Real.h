#pragma once
#include <string>


namespace Angel::Logic
{
class Number;

// An integer is a whole number element, the size depends on the CPU type
class Real
{
public:
	constexpr explicit Real(double value) : value(value) {}
	explicit Real(const class Boolean& b);
	explicit Real(const class Integer& i);
	explicit Real(const std::string_view s);
    explicit operator bool() const;
    explicit operator double() const;
	bool operator==(const Real& value) const;
	bool operator<(const Real& rhs) const;
	Real operator-() const;
    std::size_t Hash() const;
private:
	friend std::ostream& operator<<(std::ostream& os, const Real& );
	double value;
};

Number operator+(const Real& lhs, const Real& rhs);
Number operator-(const Real& lhs, const Real& rhs);
Number operator*(const Real& lhs, const Real& rhs);
Number operator/(const Real& lhs, const Real& rhs);
Number operator^(const Real& lhs, const Real& rhs);
Number log(const Real& lhs, const Real& rhs);

std::ostream& operator<<(std::ostream& os, const Real& );


}