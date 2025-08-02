#pragma once 
#include "Logic/Expression.h"
#include "Logic/Comparator.h"
#include "Logic/Internal/MultiaryImpl.h"

namespace Angel::Logic
{

template<Order OP> 
bool order(const Expression& lhs, const Expression& rhs);

template<> 
constexpr bool order<Order{L'='}>(const Expression& lhs, const Expression& rhs)
{
    return lhs == rhs;
}
 
template<> 
constexpr bool order<Order{L'≠'}>(const Expression& lhs, const Expression& rhs)
{
    return lhs != rhs;
}

template<> 
constexpr bool order<Order{L'<'}>(const Expression& lhs, const Expression& rhs)
{
    return lhs < rhs;
}

template<> 
constexpr bool order<Order{L'≤'}>(const Expression& lhs, const Expression& rhs)
{
    return lhs <= rhs;
}

template<> 
constexpr bool order<Order{L'>'}>(const Expression& lhs, const Expression& rhs)
{
    return lhs > rhs;
}

template<> 
constexpr bool order<Order{L'≥'}>(const Expression& lhs, const Expression& rhs)
{
    return lhs >= rhs;
}

template<OperatorList ALL, std::size_t I = 0>
constexpr bool dispatch_order(Operator op, const Expression& lhs, const Expression& rhs) 
{
    if constexpr (I < ALL.size()) 
    {
        if (Order(ALL[I]) == op) 
            return order<Order(ALL[I])>(lhs, rhs);
        else 
            return dispatch_order<ALL, I + 1>(op, lhs, rhs);
    }
    return false;
}

constexpr bool Order::operator()(const Expression& lhs, const Expression& rhs) const
{
    return dispatch_order<L"=≠<≤≥>">(*this, lhs, rhs);
}

}
