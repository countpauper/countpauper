#pragma once
#include "Internal/OperationBase.h"
#include "Logic/Multiary.h"
#include "Logic/Integer.h"
#include <iostream>

namespace Angel::Logic
{

class Exponentiation : public OperationBase<Exponentiation>  
{
public:
    using OperationBase<Exponentiation>::OperationBase;
    Expression Simplify() const;
    bool operator==(const Exponentiation& rhs) const { return BaseType::operator==(rhs);}
    constexpr static MultiOperator ope{L'↑'};
};

std::ostream& operator<<(std::ostream& os, const Exponentiation& operation);

}