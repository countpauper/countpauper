#pragma once 
#include "Logic/Binary.h"

namespace Angel::Logic
{

class Comparator : public BinaryOperator
{
public:
    constexpr explicit Comparator(const wchar_t tag) : BinaryOperator(tag, true) {}
    Comparator(const std::string_view tag) : BinaryOperator(tag) {} 

    constexpr bool operator()(const Expression& lhs, const Expression& rhs) const;
};

}