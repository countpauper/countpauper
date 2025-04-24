#pragma once

#include <optional>
#include "Object.h"
#include "Element.h"

namespace Angel::Logic
{

// Boolean is an element that is simply true, or false
class Boolean : public Element
{
public:
	explicit Boolean(bool v);
	bool operator==(const Expression& other) const override;
    bool operator*() const;
    operator bool() const;
    std::size_t Hash() const override;

	static std::optional<bool> Parse(const std::string& tag);
protected:
    Object Cast(const std::type_info& t, const Knowledge& k) const override;
private:
    friend std::ostream& operator<<(std::ostream& os, const Boolean& );
	bool truth;
};

std::ostream& operator<<(std::ostream& os, const Boolean& );


Object boolean(bool v);
Object boolean(const std::string_view v);

// This templated function is needed because otherwise boolean("false")
// prioritizes a cast from const char* to bool, making it boolean(true)
template <typename T>  
requires std::convertible_to<T, const std::string_view>  
Object boolean(T&& v) {  
    return boolean(std::string_view(v));
}  

}