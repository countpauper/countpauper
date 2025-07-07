#pragma once
#include "Logic/Expression.h"

namespace Angel::Logic
{

template<PrefixOperator OP> 
constexpr Expression operate(const Expression&);

template<> 
constexpr Expression operate<PrefixOperator{L'-'}>(const Expression& operand)
{
    return Integer(- *operand.Cast<Integer>());
}

template<PostfixOperator OP> 
constexpr Expression operate(const Expression&) { return Expression(); }

template<OperatorList ALL, std::size_t I = 0>
constexpr Expression dispatch_operator(Operator op, const Expression& operand) 
{
    if constexpr (I < ALL.size()) 
    {
        if (PrefixOperator(ALL[I]) == op) 
            return operate<PrefixOperator(ALL[I])>(operand);
        else if (PostfixOperator(ALL[I]) == op) 
            return operate<PostfixOperator(ALL[I])>(operand);
        else 
            return dispatch_operator<ALL, I + 1>(op, operand);
    }
    return Expression();
}

constexpr Expression PrefixOperator::operator()(const Expression& operand) const
{
    return dispatch_operator<L"-">(*this, operand);
}

}
