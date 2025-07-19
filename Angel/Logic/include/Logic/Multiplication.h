#pragma once
#include "Internal/OperationBase.h"
#include "Logic/Multiary.h"
#include "Logic/Integer.h"
#include <iostream>

namespace Angel::Logic
{

class Multiplication : public OperationBase<Multiplication>  
{
public:
    using OperationBase<Multiplication>::OperationBase;
    bool operator==(const Multiplication& rhs) const { return BaseType::operator==(rhs);}
    constexpr static Integer identity{1};
    constexpr static Integer absorb{0};
    constexpr static MultiOperator ope{L'⋅'};
    using Parient = class Operation;
};

}