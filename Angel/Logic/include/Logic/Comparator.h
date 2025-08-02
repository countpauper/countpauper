#pragma once 
#include "Logic/Operator.h"

namespace Angel::Logic
{

class Comparator : public Operator
{
public:
    constexpr explicit Comparator(const wchar_t tag, unsigned operands=2) : Operator(tag, operands, false, true)  {}
    Comparator(const std::string_view tag, unsigned operands=2) : Operator(tag, operands) {} 

    constexpr bool operator()(const Expression& lhs, const Expression& rhs) const;
};

class Order : public Comparator
{
public:
    constexpr explicit Order(const wchar_t tag) : Comparator(tag, 3) {}
    Order(const std::string_view tag) : Comparator(tag, 3) {} 

    constexpr bool operator()(const Expression& lhs, const Expression& rhs) const;
};

}