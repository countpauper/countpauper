#pragma once
#include "Internal/Operation.h"
#include "Logic/Binary.h"
#include "Logic/Integer.h"
#include <iostream>

namespace Angel::Logic
{

class Multiplication : public Operation<Multiplication>  
{
public:
    using Operation<Multiplication>::Operation;
    bool operator==(const Multiplication& rhs) const { return BaseType::operator==(rhs);}
    static const Integer initial;
    constexpr static BinaryOperator ope{L'⋅'};
};

}