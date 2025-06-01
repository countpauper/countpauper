#pragma once

#include "Logic/Expression.h"
#include "Logic/Operator.h"


namespace Angel::Logic
{

template<BinaryOperator OP> Integer& operate(Integer& lhs, const Integer& rhs);
template<> Integer& operate<BinaryOperator{L'+'}>(Integer& lhs, const Integer& rhs);
template<> Integer& operate<BinaryOperator{L'-'}>(Integer& lhs, const Integer& rhs);
template<> Integer& operate<BinaryOperator{L'*'}>(Integer& lhs, const Integer& rhs);
template<> Integer& operate<BinaryOperator{L'/'}>(Integer& lhs, const Integer& rhs);
template<> Integer& operate<BinaryOperator{L'^'}>(Integer& lhs, const Integer& rhs);

}
