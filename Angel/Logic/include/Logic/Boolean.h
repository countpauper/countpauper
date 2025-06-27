#pragma once

#include <string>

namespace Angel::Logic
{

// Boolean is an element that is simply true, or false
class Boolean 
{
public:
	explicit constexpr Boolean(bool v) : truth(v) {}
	explicit Boolean(const std::string_view tag);
    explicit Boolean(const char* tag) : Boolean(std::string_view(tag)) {}
    explicit Boolean(const class Integer& i);
    explicit Boolean(const class Tuple& c);
    explicit Boolean(const class Set& s);
	bool operator==(const Boolean& other) const;
    bool operator*() const;
    explicit operator bool() const;
    std::size_t Hash() const;
    operator std::string() const;
private:
    friend std::ostream& operator<<(std::ostream& os, const Boolean& );
	bool truth;
};

static constexpr Boolean True(true); 
static constexpr Boolean False(false); 

std::ostream& operator<<(std::ostream& os, const Boolean& );

}