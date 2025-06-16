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
    return bool(lhs) && rhs->Simplify() != Boolean(false);
}

std::size_t Pair::size() const
{
    return static_cast<std::size_t>(bool(lhs)) + 
        static_cast<std::size_t>(bool(rhs));
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

Expression Association::Simplify() const
{
    auto lhSimple = lhs->Simplify();
    auto rhSimple = rhs->Simplify();
    if (lhSimple.Is<Predicate>())
    {
        if (rhSimple == Boolean(true))
            return lhSimple;
    }
    return Association(std::move(lhSimple), std::move(rhSimple));
}

Association Association::Substitute(const Hypothesis& hypothesis) const
{
    return Association(lhs->Substitute(hypothesis), rhs->Substitute(hypothesis));
}

Expression Association::Matches(const Expression& expression, const Hypothesis& hypothesis) const
{
    if (const auto* other = expression.GetIf<Association>())
    {
        return Hypothesis{lhs->Matches(other->Left(), hypothesis), rhs->Matches(other->Right(),hypothesis)};
    }
    else 
    {
        Association simple = Substitute(hypothesis);
        return simple.Get(expression);
    }
}

Expression Association::Infer(const class Knowledge& k, const Hypothesis& hypothesis, Trace& trace) const
{
    return Association{
        std::move(*lhs),
        rhs->Infer(k, hypothesis, trace)
    };
}

Expression Association::Get(const Expression& key) const
{
    if (*lhs == key)
        return *rhs;
    else 
        return Boolean(false);
}

bool Association::IsClause() const
{
    return Left().Is<Predicate>();
}

std::ostream& operator<<(std::ostream& os, const Association& a)
{
    if (a.IsClause())
        os << a.Left() << a.ope << a.Right();
    else 
        os << a.Left() << ':' << a.Right();
    return os;
}

}