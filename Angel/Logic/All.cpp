#include "Logic/All.h"
#include "Logic/Expression.h"

namespace Angel::Logic
{
         
All::All(std::initializer_list<Expression> items) : 
    All(List(items))
{
}

All::All(Expression&& other) :
    Individual(Expression())
{
    if (!other)
        return; // anonymous

    if (other.Is<Container>())
    {
        content = std::make_unique<Expression>(std::move(other));
    }
    else if (other.Is<Tuple>())
    {
        content = std::make_unique<Expression>(std::move(other));
    }
    else 
    {
        throw std::invalid_argument(std::format("{} is an invalid operand for All operator", to_string(other)));
    }
}

All All::Simplify() const
{
    return All(content->Simplify());
}

Expression All::Matches(const Expression& expression, const Hypothesis& hypothesis) const
{
    if (const All* other = expression.GetIf<All>())
    {
        return content->Matches(**other, hypothesis);
    }
    return Boolean(false);
}

All All::Substitute(const Hypothesis& hypothesis) const
{
    return All(content->Substitute(hypothesis));
}

Expression All::Infer(const class Knowledge& k, const Hypothesis& hypothesis, Trace& trace) const
{
    return All(content->Infer(k, hypothesis, trace));
}

bool All::operator==(const All& other) const
{
    return Individual::operator==(other);
}

std::size_t All::size() const
{
    if (content)
        return content->size();
    else
        return 0;
}

const_container_iterator All::begin() const
{
    if (!content)
        return const_container_iterator();
    const auto* container = content->GetIf<Container>();
    assert(container); // shouldn't be constructed or Id should be substituted already. What do ? 
    return container->begin();
}

const_container_iterator All::end() const
{
    if (!content)
        return const_container_iterator();
    const auto* container = content->GetIf<Container>();
    assert(container); // shouldn't be constructed or Id should be substituted already. What do ? 
    return container->end();

}

std::ostream& operator<<(std::ostream& os, const All& all)
{
    os << all.ope << *all;
    return os;
}
}