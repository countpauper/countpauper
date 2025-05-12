#pragma once

#include <string>

namespace Angel::Logic
{

// Boolean is an element that is simply true, or false
class Boolean 
{
public:
	explicit Boolean(bool v);
	explicit Boolean(const std::string& tag);
	bool operator==(const Boolean& other) const;
    bool operator*() const;
    operator bool() const;
    std::size_t Hash() const;
    operator std::string() const;
private:
    friend std::ostream& operator<<(std::ostream& os, const Boolean& );
	bool truth;
};

std::ostream& operator<<(std::ostream& os, const Boolean& );

}