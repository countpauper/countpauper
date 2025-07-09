#pragma once
#include "Internal/OperationBase.h"
#include "Logic/Multiary.h"
#include "Logic/Integer.h"
#include <iostream>

namespace Angel::Logic
{

class Division : public OperationBase<Division>  
{
public:
    using OperationBase<Division>::OperationBase;
    bool operator==(const Division& rhs) const { return BaseType::operator==(rhs);}
    constexpr static MultiOperator ope{L'÷'};
};

}