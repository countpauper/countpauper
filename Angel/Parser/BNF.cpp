#include "stdafx.h"
#include "BNF.h"

namespace Angel::Parser::BNF
{

PossibleMatch Nothing::Parse(const std::string_view data) const
{
    return Match{ data };
}

PossibleMatch Literal::Parse(const std::string_view data) const
{
    if (data.substr(0,literal.size()).compare(literal)==0)
        return Match({ data.data() + literal.size() });
    else
        return PossibleMatch();
}

PossibleMatch RegularExpression::Parse(const std::string_view data) const
{
    return PossibleMatch();
}

PossibleMatch Disjunction::Parse(const std::string_view data) const
{
    return PossibleMatch();
}

PossibleMatch Sequence::Parse(const std::string_view data) const
{
    return PossibleMatch();
}

PossibleMatch Loop::Parse(const std::string_view data) const
{
    return PossibleMatch();
}

PossibleMatch Whitespace::Parse(const std::string_view data) const
{
    return PossibleMatch();
}

PossibleMatch Rule::Parse(const std::string_view data) const
{
    return expression.Parse(data);
}

PossibleMatch RuleRef::Parse(const std::string_view data) const
{
    return rule.Parse(data);
}

void Parse(const Rule& root, const std::string_view data)
{
    auto match = root.Parse(data);
    if (match)
    {
        // Check that all data is matched (should be any?)
        if (match->remaining.data() == data.data() + data.size())
            return;
        else
            throw SyntaxError(std::string("Remaining:") + match->remaining.data());
    }
    else
        throw SyntaxError(std::string("Couldn't match any rule at:") + data.data());
}

}
