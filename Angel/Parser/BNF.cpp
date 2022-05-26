#include "stdafx.h"
#include "BNF.h"
#include <regex>

namespace Angel::Parser::BNF
{

PossibleMatch Nothing::Parse(const std::string_view data) const
{
    return Match( data );
}

PossibleMatch Literal::Parse(const std::string_view data) const
{
    if (data.substr(0,literal.size()).compare(literal)==0)
        return Match( data.data() + literal.size() );
    else
        return PossibleMatch();
}

PossibleMatch RegularExpression::Parse(const std::string_view data) const
{
    const std::regex re(expression);
    std::cmatch match;
    if (std::regex_search(data.data(), match, re, std::regex_constants::match_continuous))
    {
        return Match( data.data() + match[0].length() );
    }

    return PossibleMatch();
}

PossibleMatch Disjunction::Parse(const std::string_view data) const
{
    for (const auto& e : expressions)
    {
        auto m = e->Parse(data);
        if (m)
            return m;
    }
    return PossibleMatch();
}

PossibleMatch Sequence::Parse(const std::string_view data) const
{
    auto remaining = data;
    for (const auto& e : expressions)
    {
        auto m = e->Parse(remaining);
        if (!m)
            return PossibleMatch();
        remaining = m->remaining;
    }
    return Match{ remaining };
}

PossibleMatch Loop::Parse(const std::string_view data) const
{
    auto remaining = data;
    std::vector<std::string> results;
    while (!remaining.empty())
    {
        auto m = expression->Parse(remaining);
        if (!m)
            break;
        if (m->remaining == remaining)
            break;
        auto len = m->remaining.data() - remaining.data();
        results.push_back(std::string(remaining.substr(0,len)));
        remaining = m->remaining;
    }
    auto result = Match{ remaining };

    for (auto it = results.begin(); it != results.end(); ++it)
    {
        result.result(std::string("[") + std::to_string(it - results.begin()) + "]", *it);
    }
    return result;
}

PossibleMatch Whitespace::Parse(const std::string_view data) const
{
    if (data.empty())
        return PossibleMatch();
    auto p = data.find_first_not_of(" \t\r\n");
    if (p == data.npos)
        return Match{ data.data() + data.size() };
    else if (p>0)
        return Match{ data.substr(p) };
    else
        return PossibleMatch();
}

PossibleMatch Rule::Parse(const std::string_view data) const
{
    auto m = expression->Parse(data);
    if (!m)
        return m;
    else
    {
        // TODO: prepend all existing keys in m with this name
        Match result(name, *m);
        auto len = m->remaining.data() - data.data();
        result.result(name, data.substr(0, len));
        return result;;
    }
}

PossibleMatch Ref::Parse(const std::string_view data) const
{
    return rule->Parse(data);
}

Match Parse(const Rule& root, const std::string_view data)
{
    auto possible = root.Parse(data);
    if (!possible)
        throw SyntaxError(std::string("Couldn't match ") + root.name.data() +" at:" + data.data());
    else
        return *possible;
}

}
