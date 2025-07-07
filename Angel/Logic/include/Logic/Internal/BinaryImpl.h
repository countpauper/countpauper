#pragma once

#include "Logic/Expression.h"
#include "Logic/Binary.h"
#include <array>
#include <cassert>

namespace Angel::Logic
{

template<BinaryOperator OP> 
constexpr Expression operate(const Expression&, const Expression&);
 
template<>
constexpr Expression operate<BinaryOperator{L'+'}>(const Expression& lhs, const Expression& rhs)
{
    if (const auto* lhInt = lhs.GetIf<Integer>())
    {
        return *lhInt + rhs.Cast<Integer>();
    }
    else if (const auto* lhBool = lhs.GetIf<Boolean>())
    {   // TODO this gets rediculous with more numbers. Make numbers variant, also in operator- 
        return Integer(*lhBool) + rhs.Cast<Integer>();
    }
    else if (const auto* lhContainer = lhs.GetIf<Container>())
    {
        return *lhContainer + rhs.Cast<Container>();
    }
    else if (const auto* lhSum = lhs.GetIf<Summation>())
    {
        assert(false); // possibly missing infer or shouldn't get here
        Summation sum = *lhSum;
        sum.Add(std::move(Expression(rhs)));
        return sum;
    }
    else 
    {
        assert(false); // possibly missing infer or shouldn't get here
        return Summation{lhs, rhs};
    }
}

template<>
constexpr Expression operate<BinaryOperator{L'-'}>(const Expression& lhs, const Expression& rhs)
{
    if (const auto* lhInt = lhs.GetIf<Integer>())
    {
        return *lhInt - rhs.Cast<Integer>();
    }
    else if (const auto* lhBool = lhs.GetIf<Boolean>())
    {  
        return Integer(*lhBool) - rhs.Cast<Integer>();
    }
    else if (const auto* lhContainer = lhs.GetIf<Container>())
    {
        return *lhContainer - rhs.Cast<Container>();
    }
    else if (const auto* lhSub = lhs.GetIf<Subtraction>())
    {
        Subtraction sub = *lhSub;
        sub.Add(std::move(Expression(rhs)));
        return sub;
    }
    else 
    {
        return Subtraction{lhs, rhs};
    }
}

template<>
constexpr Expression operate<BinaryOperator{L'⋅'}>(const Expression& lhs, const Expression& rhs)
{
    auto lhInt = lhs.Cast<Integer>();
    auto rhInt = rhs.Cast<Integer>();
    lhInt*=rhInt;
    return lhInt;
}

template<>
constexpr Expression operate<BinaryOperator{L'÷'}>(const Expression& lhs, const Expression& rhs)
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

template<>
constexpr Expression operate<BinaryOperator{L'∧'}>(const Expression& lhs, const Expression& rhs)
{
    if (const auto* lhCont = lhs.GetIf<Container>())
    {
        return *lhCont & rhs.Get<Container>();
    }
    auto lhBool = lhs.Get<Boolean>();
    return lhBool & rhs.Cast<Boolean>();

}


template<>
constexpr Expression operate<BinaryOperator{L'∨'}>(const Expression& lhs, const Expression& rhs)
{
    if (const auto* lhCont = lhs.GetIf<Container>())
    {
        return *lhCont | rhs.Get<Container>();
    }
    auto lhBool = lhs.Get<Boolean>();
    return lhBool | rhs.Cast<Boolean>();
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
    throw std::runtime_error(std::format("Operator {} not defined for operands of type {} and {}", 
        std::string(op), lhs.AlternativeTypeId().name(), rhs.AlternativeTypeId().name()));
}

constexpr Expression BinaryOperator::operator()(const Expression& lhs, const Expression& rhs) const
{
    return dispatch_operator<L"+-⋅÷↑∧∨">(*this, lhs, rhs);
}

}
