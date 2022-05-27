#include "stdafx.h"
#include "BNF.h"
#include "Interpreter.h"
#include <regex>

namespace Angel::Parser::BNF
{

PossibleMatch Nothing::Parse(const std::string_view data, const Interpreter&) const
{
    return Match( data );
}

PossibleMatch Literal::Parse(const std::string_view data, const Interpreter&) const
{
    if (data.substr(0,literal.size()).compare(literal)==0)
        return Match( data.substr(literal.size()) );
    else
        return PossibleMatch();
}

PossibleMatch RegularExpression::Parse(const std::string_view data, const Interpreter&) const
{
    const std::regex re(expression);
    std::cmatch match;
    if (std::regex_search(std::string(data).c_str(), match, re, std::regex_constants::match_continuous))
    {
        return Match( data.substr(match[0].length()) );
    }

    return PossibleMatch();
}

PossibleMatch Whitespace::Parse(const std::string_view data, const Interpreter&) const
{
    if (data.empty())
        return PossibleMatch();
    auto p = data.find_first_not_of(" \t\r\n");
    if (p == data.npos)
        return Match{ data.substr(data.size()) };
    else if (p>0)
        return Match{ data.substr(p) };
    else
        return PossibleMatch();
}

PossibleMatch Disjunction::Parse(const std::string_view data, const Interpreter& interpreter) const
{
    for (const auto& e : expressions)
    {
        auto m = e->Parse(data, interpreter);
        if (m)
            return m;
    }
    return PossibleMatch();
}

PossibleMatch Sequence::Parse(const std::string_view data, const Interpreter& interpreter) const
{
    auto remaining = data;
    for (const auto& e : expressions)
    {
        auto m = e->Parse(remaining, interpreter);
        if (!m)
            return PossibleMatch();
        remaining = m->remaining;
    }
    return Match{ remaining };
}

PossibleMatch Loop::Parse(const std::string_view data, const Interpreter& interpreter) const
{
    auto remaining = data;
    std::any allInterpretations;
    int index = 0;
    while (!remaining.empty())
    {
        auto m = expression->Parse(remaining, interpreter);
        if (!m)
            break;
        if (m->remaining == remaining)
            break;

        auto len = m->remaining.data() - remaining.data();
        auto parsed = remaining.substr(0,len);
        std::string indexKey = std::string("[") + std::to_string(index++) + "]";
        allInterpretations = interpreter.Merge(allInterpretations, interpreter.Interpret(indexKey, m->interpretation, parsed));
        remaining = m->remaining;
    }
    return Match{ remaining, allInterpretations };
}

PossibleMatch Rule::Parse(const std::string_view data, const Interpreter& interpreter) const
{
    auto m = expression->Parse(data, interpreter);
    if (!m)
        return m;
    else
    {
        auto len = m->remaining.data() - data.data();
        std::string_view parsed = data.substr(0, len);
        return Match{ m->remaining, interpreter.Interpret(name, m->interpretation, parsed) };
    }
}

PossibleMatch Ref::Parse(const std::string_view data, const Interpreter& interpreter) const
{
    return rule->Parse(data, interpreter);
}

Match Parse(const Rule& root, const Interpreter& interpreter, const std::string_view data)
{
    auto possible = root.Parse(data, interpreter);
    if (!possible)
        throw SyntaxError(std::string("Couldn't match ") + std::string(root.name) +" at:" + std::string(data));
    else
        return *possible;
}

}
