#pragma once 
#include "Logic/Operator.h"

namespace Angel::Logic
{
    
class BinaryOperator : public Operator 
{
public: 
    static constexpr unsigned operands = 2;
    constexpr explicit BinaryOperator(const wchar_t tag) : Operator(tag, operands) {}
    constexpr explicit BinaryOperator(const std::string_view tag) : Operator(tag, operands) {}
};

}