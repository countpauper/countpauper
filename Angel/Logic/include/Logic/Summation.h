#pragma once
#include "Internal/OperationWithBase.h"
#include "Logic/Binary.h"
#include "Logic/Integer.h"
#include <iostream>

namespace Angel::Logic
{

// A Summation is a mathemetatical operator, which returns all values added together
// It is currently a Tuple (ordered, non-unique), but technically it could be unordered 
class Summation : public OperationWithBase<Summation>  
{
public:
    using OperationWithBase<Summation>::OperationWithBase;
    bool operator==(const Summation& rhs) const { return BaseType::operator==(rhs);}
    static const Integer initial;
    constexpr static BinaryOperator ope{L'+'};
};

std::ostream& operator<<(std::ostream& os, const Summation& operation);

}