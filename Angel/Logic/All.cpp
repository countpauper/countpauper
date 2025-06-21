#include "Logic/All.h"
#include "Logic/Expression.h"

namespace Angel::Logic
{
         
All::All(std::initializer_list<Expression> items) : 
    All(List(items))
{
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

std::ostream& operator<<(std::ostream& os, const All& all)
{
    os << all.ope << *all;
    return os;
}
}