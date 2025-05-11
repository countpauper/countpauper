#pragma once
#include <string>
#include <optional>
#include "Object.h"

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
	bool operator==(const Expression& value) const;
    std::size_t Hash() const;
    long operator*() const;
	static std::optional <long> Parse(const std::string& tag);
protected:
    Object Cast(const std::type_info& t, const Knowledge& k) const;
private:
	friend std::ostream& operator<<(std::ostream& os, const Integer& );
	long value;
};

std::ostream& operator<<(std::ostream& os, const Integer& );

Object integer(long value);
Object integer(const std::string_view value);

}
}