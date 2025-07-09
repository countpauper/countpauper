#pragma once 
#include "Logic/Operator.h"

namespace Angel::Logic
{
    
class MultiOperator : public Operator 
{
public: 
    static constexpr unsigned operands = 3;
    constexpr explicit MultiOperator(const wchar_t tag) : Operator(tag, operands) {}
    MultiOperator(const std::string_view tag) : Operator(tag, operands) {}
    constexpr Expression operator()(const Expression& lhs, const Expression& rhs) const;
protected:
    constexpr explicit MultiOperator(const wchar_t tag, bool comparator) : Operator(tag, operands, false, comparator) {}
};

}