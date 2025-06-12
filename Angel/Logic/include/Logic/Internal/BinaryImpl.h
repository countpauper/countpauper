#pragma once

#include "Logic/Expression.h"
#include "Logic/Binary.h"
#include <array>

namespace Angel::Logic
{

template<BinaryOperator OP> 
constexpr Expression operate(const Expression&, const Expression&);
 
template<>
constexpr Expression operate<BinaryOperator{L'+'}>(const Expression& lhs, const Expression& rhs)
{
    auto lhInt = lhs.Cast<Integer>();
    auto rhInt = rhs.Cast<Integer>();
    lhInt+=rhInt;
    return lhInt;
}

template<>
constexpr Expression operate<BinaryOperator{L'-'}>(const Expression& lhs, const Expression& rhs)
{
    auto lhInt = lhs.Cast<Integer>();
    auto rhInt = rhs.Cast<Integer>();    
    lhInt-=rhInt;
    return lhInt;
}

template<>
constexpr Expression operate<BinaryOperator{L'*'}>(const Expression& lhs, const Expression& rhs)
{
    auto lhInt = lhs.Cast<Integer>();
    auto rhInt = rhs.Cast<Integer>();
    lhInt*=rhInt;
    return lhInt;
}

template<>
constexpr Expression operate<BinaryOperator{L'/'}>(const Expression& lhs, const Expression& rhs)
{
    auto lhInt = lhs.Cast<Integer>();
    auto rhInt = rhs.Cast<Integer>();
    lhInt/=rhInt;
    return lhInt;
}

template<>
constexpr Expression operate<BinaryOperator{L'↑'}>(const Expression& lhs, const Expression& rhs)
{
    auto lhInt = lhs.Cast<Integer>();
    auto rhInt = rhs.Cast<Integer>();
    lhInt^=rhInt;
    return lhInt;
}

template<std::size_t N>
struct OperatorList
{
    wchar_t value[N];
    constexpr OperatorList(const wchar_t (&str)[N]) {
        std::copy_n(str, N, value);
    }
    constexpr wchar_t operator[](unsigned idx ) const { return value[idx]; }
    constexpr std::size_t size() const { return N-1; } 
};

template<OperatorList ALL, std::size_t I = 0>
constexpr Expression dispatch_operator(Operator op, const Expression& lhs, const Expression& rhs) 
{
    if constexpr (I < ALL.size()) 
    {
        if (BinaryOperator(ALL[I]) == op) 
            return operate<BinaryOperator(ALL[I])>(lhs, rhs);
        else 
            return dispatch_operator<ALL, I + 1>(op, lhs, rhs);
    }
    return Expression();
}

constexpr Expression BinaryOperator::operator()(const Expression& lhs, const Expression& rhs) const
{
    return dispatch_operator<L"+-*/↑">(*this, lhs, rhs);
}

}
