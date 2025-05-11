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
protected:
    // Object Cast(const std::type_info& t, const Knowledge& k) const;
private:
    friend std::ostream& operator<<(std::ostream& os, const Boolean& );
	bool truth;
};

std::ostream& operator<<(std::ostream& os, const Boolean& );

/*
Object boolean(bool v);
Object boolean(const std::string_view v);

// This templated function is needed because otherwise boolean("false")
// prioritizes a cast from const char* to bool, making it boolean(true)
template <typename T>  
requires std::convertible_to<T, const std::string_view>  
Object boolean(T&& v) {  
    return boolean(std::string_view(v));
}  
*/

}