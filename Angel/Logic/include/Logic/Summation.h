#pragma once
#include "Logic/Operator.h"
#include "Internal/Operation.h"
#include <iostream>

namespace Angel::Logic
{

// A Summation is a mathemetatical operator, which returns all values added together
// It is currently a Collection (ordered, non-unique), but technically it could be unordered 
class Summation : public Operation<Summation>  
{
public:
    using Operation<Summation>::Operation;
    bool operator==(const Summation& rhs) const { return Operation<Summation>::operator==(rhs);}
    constexpr static long initial = 0;
    constexpr static BinaryOperator ope{L'+'};
};

}