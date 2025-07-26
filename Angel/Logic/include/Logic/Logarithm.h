#pragma once
#include "Internal/OperationBase.h"
#include "Logic/Multiary.h"
#include "Logic/Integer.h"
#include <iostream>

namespace Angel::Logic
{

class Logarithm : public OperationBase<Logarithm>  
{
public:
    using OperationBase<Logarithm>::OperationBase;
    bool operator==(const Logarithm& rhs) const { return BaseType::operator==(rhs);}
    constexpr static MultiOperator ope{L'↓'};
};

std::ostream& operator<<(std::ostream& os, const Logarithm& operation);

}