#include "Logic/Association.h"
#include "Logic/Expression.h"

namespace Angel::Logic
{


class Expression;

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

Pair::Pair(Expression&& lhs, Expression&& rhs) : 
    lhs(std::make_unique<Expression>(std::move(lhs))),
    rhs(std::make_unique<Expression>(std::move(rhs)))
{
}

Pair::operator bool() const
{
    return bool(lhs);
}

std::size_t Pair::size() const
{
    return static_cast<std::size_t>(bool(lhs)) + 
        static_cast<std::size_t>(bool(rhs));
}

const Expression& Pair::Left() const
{
    return *lhs;
}

const Expression& Pair::Right() const
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

Association& Association::operator=(const Association& o)
{
    lhs = std::make_unique<Expression>(*o.lhs);
    rhs = std::make_unique<Expression>(*o.rhs);
    return *this;
}

Association& Association::operator=(Association&& o)
{
    lhs.reset();
    rhs.reset();
    std::swap(lhs, o.lhs);
    std::swap(rhs, o.rhs);
    return *this;
}


Expression Association::Matches(const Expression& expression, const Variables& vars) const
{
    // TODO what does it mean? does only lhs need to match? 
    return Boolean(false);

}

Expression Association::Infer(const class Knowledge& k, const Variables& substitutions) const
{
    return Association{
        std::move(*lhs),
        rhs->Infer(k, substitutions)
    };
}

Expression Association::Get(const Expression& key) const
{
    if (*lhs == key)
        return *rhs;
    else 
        return Boolean(false);
}

std::ostream& operator<<(std::ostream& os, const Association& a)
{
    os << a.Left() << a.ope << a.Right();
    return os;
}

}