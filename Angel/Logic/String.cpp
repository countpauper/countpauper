#include "Logic/String.h"
#include "Logic/Element.h"
#include "Logic/VariantUtils.h"
#include "Logic/Expression.h"
#include <sstream>

namespace Angel::Logic
{

template<typename T>
requires is_alternative<T, ElementVariant>
std::string element_to_string(const T& element)
{
    std::stringstream ss;
    ss << element;
    return ss.str();
}
String::String(const std::string_view value) :
    value(value)
{
}

String::String(const class Boolean& b) :
    value(element_to_string(b))
{
}

String::String(const class Integer& i) :
    value(element_to_string(i))
{
}
    
String::operator bool() const
{
    return !value.empty();
}

bool String::operator==(const String& rhs) const
{
    return value == rhs.value;
}
    
std::size_t String::Hash() const
{
    std::hash<std::string> hasher;
    return hasher(value);
}
    
const std::string_view String::operator*() const
{
    return value;
}

std::ostream& operator<<(std::ostream& os, const String& str)
{
    os << *str;
    return os;
}

}