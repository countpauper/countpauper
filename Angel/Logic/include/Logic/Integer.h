#pragma once
#include <string>


namespace Angel::Logic
{
class Number;

// An integer is a whole number element, the size depends on the CPU type
class Integer
{
public:
	constexpr explicit Integer(long value) : value(value) {}
	explicit Integer(const class Boolean& b);
	explicit Integer(const class Real& r);
	explicit Integer(const std::string_view s);
    explicit operator bool() const;
    explicit operator long() const;
	bool operator==(const Integer& value) const;
	bool operator<(const Integer& rhs) const;
	Integer operator-() const;
	
    std::size_t Hash() const;
private:
	friend std::ostream& operator<<(std::ostream& os, const Integer& );
	long value;
};

Number operator+(const Integer& lhs, const Integer& rhs);
Number operator-(const Integer& lhs, const Integer& rhs);
Number operator*(const Integer& lhs, const Integer& rhs);
Number operator/(const Integer& lhs, const Integer& rhs);
Number operator^(const Integer& lhs, const Integer& rhs);
Number log(const Integer& lhs, const Integer& rhs);

std::ostream& operator<<(std::ostream& os, const Integer& );


}