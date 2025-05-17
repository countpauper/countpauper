#pragma once 
#include <string_view>
#include <cstdint> 

namespace Angel::Logic
{

class Operator 
{
public:
    constexpr explicit Operator(wchar_t tag=0) :
        code(tag)
    {
    }
    explicit Operator(const std::string_view tag);

    bool operator==(const Operator& rhs) const;
    explicit operator bool() const;
    operator std::string() const;
    unsigned Precedence() const;
private:
    uint32_t code;  // it's just the signle character tag's unicode. 
};

std::ostream& operator<<(std::ostream& os, const Operator& list);

}