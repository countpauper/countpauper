#pragma once
#include "Internal/OperationWithBase.h"
#include "Logic/Binary.h"
#include "Logic/Integer.h"
#include <iostream>

namespace Angel::Logic
{

class Division : public OperationWithBase<Division>  
{
public:
    using OperationWithBase<Division>::OperationWithBase;
    bool operator==(const Division& rhs) const { return BaseType::operator==(rhs);}
    constexpr static BinaryOperator ope{L'÷'};
};

}