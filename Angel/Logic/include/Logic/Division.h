#pragma once
#include "Internal/OperationBase.h"
#include "Logic/Binary.h"
#include "Logic/Integer.h"
#include <iostream>

namespace Angel::Logic
{

class Division : public OperationBase<Division>  
{
public:
    using OperationBase<Division>::OperationBase;
    bool operator==(const Division& rhs) const { return BaseType::operator==(rhs);}
    constexpr static BinaryOperator ope{L'÷'};
};

}