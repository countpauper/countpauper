#pragma once
#include <string>


namespace Angel
{
namespace Logic
{

// An integer is a whole number element, the size depends on the CPU type
class Integer
{
public:
	explicit Integer(long value);
    operator bool() const;
	bool operator==(const Integer& value) const;
    std::size_t Hash() const;
    long operator*() const;
	static Integer Parse(const std::string& tag);
private:
	friend std::ostream& operator<<(std::ostream& os, const Integer& );
	long value;
};

std::ostream& operator<<(std::ostream& os, const Integer& );

}
}