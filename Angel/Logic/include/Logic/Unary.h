#pragma once 
#include "Logic/Operator.h"

namespace Angel::Logic
{

class UnaryOperator : public Operator 
{
public: 
    static constexpr unsigned operands = 1;
protected:
    constexpr explicit UnaryOperator(const wchar_t tag, bool postfix, bool comparator) : Operator(tag, 1, postfix, comparator)  {}
    UnaryOperator(const std::string_view tag);
};

class PrefixOperator : public UnaryOperator
{
public:
    constexpr explicit PrefixOperator(const wchar_t tag) : UnaryOperator(tag, false, false)  {}
    PrefixOperator(const std::string_view tag) : UnaryOperator(tag) {}

};

class PostfixOperator : public UnaryOperator
{
public:
    constexpr explicit PostfixOperator(const wchar_t tag) : UnaryOperator(tag, true, false)  {}
    PostfixOperator(const std::string_view tag) : UnaryOperator(tag) {}
};


class Filter : public UnaryOperator 
{
public:
    constexpr explicit Filter(const wchar_t tag) : UnaryOperator(tag, false, true)  {}
    Filter(const std::string_view tag) : UnaryOperator(tag) {} 
};

}