#pragma once
#include "Logic/Operator.h"
#include "Internal/OperationWithBase.h"
#include "Logic/Integer.h"
#include <iostream>

namespace Angel::Logic
{

class Subtraction : public OperationWithBase<Subtraction>  
{
public:
    using OperationWithBase<Subtraction>::OperationWithBase;
    bool operator==(const Subtraction& rhs) const { return OperationWithBase<Subtraction>::operator==(rhs);}
    static const Integer initial;
    constexpr static BinaryOperator ope{L'-'};
};

std::ostream& operator<<(std::ostream& os, const Subtraction& subtraction);

}