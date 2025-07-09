#pragma once
#include "Logic/Multiary.h"
#include "Internal/OperationBase.h"
#include "Logic/Integer.h"
#include <iostream>

namespace Angel::Logic
{

class Subtraction : public OperationBase<Subtraction>  
{
public:
    using OperationBase<Subtraction>::OperationBase;
    bool operator==(const Subtraction& rhs) const { return BaseType::operator==(rhs);}
    constexpr static MultiOperator ope{L'-'};
};

std::ostream& operator<<(std::ostream& os, const Subtraction& subtraction);

}