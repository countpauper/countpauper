#pragma once
#include "Logic/Operator.h"
#include "Logic/Integer.h"
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
    static const Integer initial;
    constexpr static BinaryOperator ope{L'+'};
};

std::ostream& operator<<(std::ostream& os, const Summation& summation);

}