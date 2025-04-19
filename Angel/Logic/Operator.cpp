#include "Logic/Operator.h"

namespace Angel::Logic
{


std::size_t Unary::Hash() const
{
    return operand.Hash();
}

std::size_t Binary::Hash() const
{
    return left.Hash() ^ right.Hash();
}

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

std::size_t Nary::Hash() const
{
    std::size_t result = 0; 
    for(const auto& o: operands)
        result ^ std::hash<Object>()(o);
    return result;
}


}