#include "Logic/Operator.h"

namespace Angel::Logic
{

Nary::Nary(const Operands&)
{

}

Nary::operator bool() const 
{
    return !operands.empty();
}

void Nary::Add(Object&&)
{

}
}