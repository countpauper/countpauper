#pragma once 
#include "Logic/Expression.h"
#include "Logic/Comparator.h"
#include "Logic/Internal/MultiaryImpl.h"

namespace Angel::Logic
{

template<Comparator OP> 
bool compare(const Expression& lhs, const Expression& rhs);

template<> 
constexpr bool compare<Comparator{L'='}>(const Expression& lhs, const Expression& rhs)
{
    return lhs == rhs;
}
 
template<> 
constexpr bool compare<Comparator{L'≠'}>(const Expression& lhs, const Expression& rhs)
{
    return lhs != rhs;
}

template<> 
constexpr bool compare<Comparator{L'<'}>(const Expression& lhs, const Expression& rhs)
{
    return lhs < rhs;
}

template<> 
constexpr bool compare<Comparator{L'≤'}>(const Expression& lhs, const Expression& rhs)
{
    return lhs <= rhs;
}

template<> 
constexpr bool compare<Comparator{L'>'}>(const Expression& lhs, const Expression& rhs)
{
    return lhs > rhs;
}

template<> 
constexpr bool compare<Comparator{L'≥'}>(const Expression& lhs, const Expression& rhs)
{
    return lhs >= rhs;
}

template<OperatorList ALL, std::size_t I = 0>
constexpr bool dispatch_compare(Operator op, const Expression& lhs, const Expression& rhs) 
{
    if constexpr (I < ALL.size()) 
    {
        if (Comparator(ALL[I]) == op) 
            return compare<Comparator(ALL[I])>(lhs, rhs);
        else 
            return dispatch_compare<ALL, I + 1>(op, lhs, rhs);
    }
    return false;
}

constexpr bool Comparator::operator()(const Expression& lhs, const Expression& rhs) const
{
    return dispatch_compare<L"=≠<≤≥>">(*this, lhs, rhs);
}

}
