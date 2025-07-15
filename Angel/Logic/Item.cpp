#include "Logic/Item.h"
#include "Logic/Expression.h"
#include "Logic/Expression.h"

namespace Angel::Logic
{


Item& Item::operator=(const Item& o)
{
    lhs = std::make_unique<Expression>(*o.lhs);
    rhs = std::make_unique<Expression>(*o.rhs);
    return *this;
}

Item& Item::operator=(Item&& o)
{
    lhs.reset();
    rhs.reset();
    std::swap(lhs, o.lhs);
    std::swap(rhs, o.rhs);
    return *this;
}

Expression Item::Simplify() const
{
    auto lhSimple = lhs->Simplify();
    auto rhSimple = rhs->Simplify();
    if (const auto* container = lhSimple.GetIf<Container>())
    {
        return container->GetItem(std::move(rhSimple));
    }
    else 
    {
        return Item(std::move(lhSimple), std::move(rhSimple));
    }
}

Item Item::Substitute(const Hypothesis& hypothesis) const
{
    auto lhSubstitute = lhs->Substitute(hypothesis);
    auto rhsSubstitue = rhs->Substitute(hypothesis);
    return Item(lhs->Substitute(hypothesis), rhs->Substitute(hypothesis));
}

Expression Item::Matches(const Expression& expression, const Hypothesis& hypothesis) const
{
    Item substituted = Substitute(hypothesis);
    return Equal{std::move(substituted), Expression(expression)};
}


Expression Item::Infer(const class Knowledge& k, const Hypothesis& hypothesis, Trace& trace) const
{
    return Item(
        lhs->Infer(k, hypothesis, trace),
        rhs->Infer(k, hypothesis, trace)
    );
}

std::ostream& operator<<(std::ostream& os, const Item& a)
{
    os << a.Left() << a.ope << a.Right();
    return os;
}

}