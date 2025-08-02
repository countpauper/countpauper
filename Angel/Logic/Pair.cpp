#include "Logic/Internal/Pair.h"
#include "Logic/Expression.h"

namespace Angel::Logic
{


Pair::Pair(const Pair& o) : 
    lhs(std::make_unique<Expression>(*o.lhs)),
    rhs(std::make_unique<Expression>(*o.rhs))
{
}

Pair::Pair(Pair&& o) : 
    lhs(std::move(o.lhs)),
    rhs(std::move(o.rhs))
{
}

Pair::operator bool() const
{
    return bool(lhs) && *rhs != False;
}

std::size_t Pair::size() const
{
    return static_cast<std::size_t>(bool(lhs)) + 
        static_cast<std::size_t>(bool(rhs));
}


Expression Pair::RemoveLeft()
{
    if (!lhs)
        return Expression();
    Expression result = std::move(*lhs);
    lhs.reset();
    return std::move(result);
}

void Pair::AddLeft(Expression&& operand)   
{
    if (lhs)
        throw std::runtime_error("Pair buffer overflow");
    lhs = std::make_unique<Expression>(std::move(operand));
}

Set Pair::Assumptions() const
{
    return Left().Assumptions() + Right().Assumptions();
}


const Expression& Pair::Left() const
{
    return *lhs;
}

Expression& Pair::Left()
{
    return *lhs;
}

const Expression& Pair::Right() const
{
    return *rhs;
}

Expression& Pair::Right()
{
    return *rhs;
}

bool Pair::operator==(const Pair& other) const
{
    if (!lhs && bool(other.lhs))
        return false;
    if (!rhs && bool(other.rhs))
        return false;
    return Left() == other.Left() && 
        Right() == other.Right();
}

std::size_t Pair::Hash() const
{
    std::hash<Expression> hasher;
    std::size_t result = 0;
    if (lhs)
        result ^= hasher(*lhs);
    if (rhs)
        result ^= hasher(*rhs);
    return result;
}

}