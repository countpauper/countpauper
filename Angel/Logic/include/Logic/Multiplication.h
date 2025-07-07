#pragma once
#include "Internal/OperationWithBase.h"
#include "Logic/Binary.h"
#include "Logic/Integer.h"
#include <iostream>

namespace Angel::Logic
{

class Multiplication : public OperationWithBase<Multiplication>  
{
public:
    using OperationWithBase<Multiplication>::OperationWithBase;
    bool operator==(const Multiplication& rhs) const { return BaseType::operator==(rhs);}
    static const Integer initial;
    constexpr static BinaryOperator ope{L'⋅'};
};

}