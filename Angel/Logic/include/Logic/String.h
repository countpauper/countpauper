#pragma once
#include <string>

namespace Angel::Logic
{

// A string is utf8 text
class String
{
public:
	explicit String(const std::string_view value);
	explicit String(const class Boolean& b);
	explicit String(const class Integer& b);
    explicit operator bool() const;
	bool operator==(const String& value) const;
    std::size_t Hash() const;
    const std::string_view operator*() const;
private:
	std::string value;
};

std::ostream& operator<<(std::ostream& os, const String& );

}