#pragma once
#include "Logic/Binary.h"
#include "Internal/OperationWithBase.h"
#include "Logic/Integer.h"
#include <iostream>

namespace Angel::Logic
{

class Subtraction : public OperationWithBase<Subtraction>  
{
public:
    using OperationWithBase<Subtraction>::OperationWithBase;
    bool operator==(const Subtraction& rhs) const { return BaseType::operator==(rhs);}
    static const Integer initial;
    constexpr static BinaryOperator ope{L'-'};
};

std::ostream& operator<<(std::ostream& os, const Subtraction& subtraction);

}