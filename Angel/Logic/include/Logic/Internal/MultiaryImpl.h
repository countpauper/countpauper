#pragma once

#include "Logic/Expression.h"
#include "Logic/Multiary.h"
#include <array>
#include <cassert>

namespace Angel::Logic
{

template<MultiOperator OP> 
constexpr Expression operate(const Expression&, const Expression&);
 
template<>
constexpr Expression operate<MultiOperator{L'+'}>(const Expression& lhs, const Expression& rhs)
{
    if (const auto* lhNumber = lhs.GetIf<Number>())
    {
        return *lhNumber + rhs.Cast<Number>();
    }
    else if (const auto* lhContainer = lhs.GetIf<Container>())
    {
        return *lhContainer + rhs.Cast<Container>();
    }
    else 
    {
        assert(false); // possibly missing infer or shouldn't get here. Or perhaps during simplify? 
        return Summation{lhs, rhs};
    }
}

template<>
constexpr Expression operate<MultiOperator{L'-'}>(const Expression& lhs, const Expression& rhs)
{
    if (const auto* lhNumber = lhs.GetIf<Number>())
    {
        return *lhNumber - rhs.Cast<Number>();
    }
    else  if (const auto* lhContainer = lhs.GetIf<Container>())
    {
        return *lhContainer - rhs.Cast<Container>();
    }
    else 
    {
        return Subtraction{lhs, rhs};
    }
}

template<>
constexpr Expression operate<MultiOperator{L'⋅'}>(const Expression& lhs, const Expression& rhs)
{
    auto lhNumber = lhs.Cast<Number>();
    auto rhNumber = rhs.Cast<Number>();
    return lhNumber * rhNumber;
}

template<>
constexpr Expression operate<MultiOperator{L'÷'}>(const Expression& lhs, const Expression& rhs)
{
    auto lhNumber = lhs.Cast<Number>();
    auto rhNumber = rhs.Cast<Number>();
    return lhNumber / rhNumber;
}

template<>
constexpr Expression operate<MultiOperator{L'↑'}>(const Expression& lhs, const Expression& rhs)
{
    auto lhNumber = lhs.Cast<Number>();
    auto rhNumber = rhs.Cast<Number>();
    return lhNumber ^ rhNumber;
}

template<>
constexpr Expression operate<MultiOperator{L'↓'}>(const Expression& lhs, const Expression& rhs)
{
    auto lhNumber = lhs.Cast<Number>();
    auto rhNumber = rhs.Cast<Number>();
    return lhNumber.log(rhNumber);
}

template<>
constexpr Expression operate<MultiOperator{L'∧'}>(const Expression& lhs, const Expression& rhs)
{
    if (const auto* lhCont = lhs.GetIf<Container>())
    {
        return *lhCont & rhs.Get<Container>();
    }
    else if (const auto* lhBool = lhs.GetIf<Boolean>())
    {
        return *lhBool & rhs;
    }
    else if (rhs == True)
    {
        return True;
    }
    else 
    {
        return Conjunction{lhs, rhs};
    }
}


template<>
constexpr Expression operate<MultiOperator{L'∨'}>(const Expression& lhs, const Expression& rhs)
{
    if (const auto* lhCont = lhs.GetIf<Container>())
    {
        return *lhCont | rhs.Get<Container>();
    }
    else if (rhs == False)
    {
        return lhs;
    }
    else if (lhs == False) 
    {
        return rhs;
    }
    auto lhBool = lhs.Get<Boolean>();
    return lhBool | rhs;
}


template<>
constexpr Expression operate<MultiOperator{L'⊕'}>(const Expression& lhs, const Expression& rhs)
{
    if (const auto* lhCont = lhs.GetIf<Container>())
    {
        return *lhCont ^ rhs.Get<Container>();
    }
    auto lhBool = lhs.TryCast<Boolean>();
    if (lhBool)
    {
        if (*lhBool)
            return Negation(rhs).Simplify();        
        else 
            return rhs;
    }
    auto rhBool = rhs.TryCast<Boolean>();
    if (rhBool)
    {
        if (*rhBool)
            return Negation(lhs).Simplify();
        else
            return lhs;
    }
    return Exclusion{lhs, rhs};        
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
        if (MultiOperator(ALL[I]) == op) 
            return operate<MultiOperator(ALL[I])>(lhs, rhs);
        else 
            return dispatch_operator<ALL, I + 1>(op, lhs, rhs);
    }
    throw std::runtime_error(std::format("Operator {} not defined for operands of type {} and {}", 
        std::string(op), lhs.AlternativeTypeId().name(), rhs.AlternativeTypeId().name()));
}

constexpr Expression MultiOperator::operator()(const Expression& lhs, const Expression& rhs) const
{
    return dispatch_operator<L"+-⋅÷↑↓∧∨⊕">(*this, lhs, rhs);
}

}
