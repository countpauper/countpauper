#pragma once 
#include "Logic/Multiary.h"

namespace Angel::Logic
{

class Comparator : public MultiOperator
{
public:
    constexpr explicit Comparator(const wchar_t tag) : MultiOperator(tag, true) {}
    Comparator(const std::string_view tag) : MultiOperator(tag) {} 

    constexpr bool operator()(const Expression& lhs, const Expression& rhs) const;
};

}