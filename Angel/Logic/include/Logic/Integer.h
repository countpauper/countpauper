#pragma once
#include <string>


namespace Angel::Logic
{

// An integer is a whole number element, the size depends on the CPU type
class Integer
{
public:
	explicit Integer(long value);
	explicit Integer(const class Boolean& b);
	explicit Integer(const std::string_view s);
    explicit operator bool() const;
	bool operator==(const Integer& value) const;
	Integer operator+=(const Integer& o);
	Integer operator-=(const Integer& o);
	Integer operator*=(const Integer& o);
	Integer operator/=(const Integer& o);
	Integer operator^=(const Integer& o);
	bool operator<(const Integer& rhs) const;

    std::size_t Hash() const;
    long operator*() const;
	static Integer Parse(const std::string& tag);
private:
	friend std::ostream& operator<<(std::ostream& os, const Integer& );
	long value;
};

Integer operator+(Integer lhs, const Integer& rhs) { return lhs+=rhs; }
Integer operator-(Integer lhs, const Integer& rhs) { return lhs-=rhs; }
Integer operator*(Integer lhs, const Integer& rhs) { return lhs*=rhs; }
Integer operator/(Integer lhs, const Integer& rhs) { return lhs/=rhs; }
Integer operator^(Integer lhs, const Integer& rhs) { return lhs^=rhs; }

std::ostream& operator<<(std::ostream& os, const Integer& );


}