#include "Logic/Association.h"
#include "Logic/Expression.h"

namespace Angel::Logic
{

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
    if (rhSimple.GetOperator().IsComparator() && rhSimple.size()==1)
    {
        rhSimple = rhSimple.Matches(lhSimple, {}).Simplify();
    }
    if (IsClause())
    {
        if (rhSimple == True)
            return lhSimple;
    }

    return Association(std::move(lhSimple), std::move(rhSimple));
}

Association Association::Substitute(const Hypothesis& hypothesis) const
{
    auto lhSubstitute = lhs->Substitute(hypothesis);
    auto rhsSubstitue = rhs->Substitute(hypothesis);
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
        Association substituted = Substitute(hypothesis);
        return substituted.Get(expression);
    }
}

Expression Association::Infer(const class Knowledge& k, const Hypothesis& hypothesis, Trace& trace) const
{
    return Association{
        std::move(*lhs),
        rhs->Infer(k, hypothesis, trace)
    };
}

const_container_iterator Association::begin() const
{
    assert(false); // really iterate over two values?
    return const_container_iterator();
}

const_container_iterator Association::end() const
{
    return const_container_iterator();
}

Expression Association::Get(const Expression& key) const
{
    auto hypothsis = lhs->Matches(key, {}).Simplify();
    if (hypothsis==True)
        return *rhs;
    if (hypothsis==False)
        return hypothsis;
    auto right = *rhs;
    return Association(std::move(right),
        std::move(hypothsis));
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