#pragma once
#include "Internal/OperationBase.h"
#include "Logic/Binary.h"
#include "Logic/Integer.h"
#include <iostream>

namespace Angel::Logic
{

// A Summation is a mathemetatical operator, which returns all values added together
// It is currently a Tuple (ordered, non-unique), but technically it could be unordered 
class Summation : public OperationBase<Summation>  
{
public:
    using OperationBase<Summation>::OperationBase;
    bool operator==(const Summation& rhs) const { return BaseType::operator==(rhs);}
    constexpr static Integer initial{ 0 };
    constexpr static BinaryOperator ope{L'+'};
};

std::ostream& operator<<(std::ostream& os, const Summation& operation);

}