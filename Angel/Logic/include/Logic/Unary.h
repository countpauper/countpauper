#pragma once 
#include "Logic/Operator.h"

namespace Angel::Logic
{

class UnaryOperator : public Operator 
{
public: 
    static constexpr unsigned operands = 1;

protected:
    constexpr explicit UnaryOperator(const wchar_t tag, bool postfix) : Operator(tag, 1, postfix)  {}
    UnaryOperator(const std::string_view tag);
};

class PrefixOperator : public UnaryOperator
{
public:
    constexpr explicit PrefixOperator(const wchar_t tag) : UnaryOperator(tag, false)  {}
    PrefixOperator(const std::string_view tag) : UnaryOperator(tag) {}
    constexpr Expression operator()(const Expression& operand) const;
};

class PostfixOperator : public UnaryOperator
{
public:
    constexpr explicit PostfixOperator(const wchar_t tag) : UnaryOperator(tag, true)  {}
    PostfixOperator(const std::string_view tag) : UnaryOperator(tag) {}
};

std::ostream& operator<<(std::ostream& os, const UnaryOperator& op);

}