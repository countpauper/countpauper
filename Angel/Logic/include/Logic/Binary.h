#pragma once 
#include "Logic/Operator.h"

namespace Angel::Logic
{
    
class BinaryOperator : public Operator 
{
public: 
    static constexpr unsigned operands = 2;
    constexpr explicit BinaryOperator(const wchar_t tag) : Operator(tag, operands) {}
    BinaryOperator(const std::string_view tag);
    constexpr Expression operator()(const Expression& lhs, const Expression& rhs) const;
};

std::ostream& operator<<(std::ostream& os, const BinaryOperator& op);

}