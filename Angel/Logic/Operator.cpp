#include "Logic/Operator.h"
#include <assert.h>

namespace Angel::Logic
{

    /*
Match Operator::Matching(const Expression& other, const Variables& substitutions) const
{
    assert(false); // the only way to match is to first Infer the Element 
    // then match with that
    return NoMatch;
}

const Object* Operator::Condition() const
{
    assert(false); // how did this get matched? 
    return nullptr;
}

std::size_t Unary::Hash() const
{
    return operand.Hash();
}

std::size_t Binary::Hash() const
{
    return left.Hash() ^ right.Hash();
}

Nary::Nary(const Operands& operands) :
    operands(operands)
{
}

Nary::operator bool() const 
{
    return !operands.empty();
}

void Nary::Add(Object&& o)
{
    operands.emplace_back(o);
}

std::size_t Nary::Hash() const
{
    std::size_t result = 0; 
    for(const auto& o: operands)
        result ^ std::hash<Object>()(o);
    return result;
}
    */


}