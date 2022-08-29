#include "stdafx.h"
#include "BNF.h"
#include "Parser.h"
#include "StringEncoding.h"
#include <regex>

namespace Angel::Parser::BNF
{

PossibleMatch Nothing::Parse(const std::string_view data, const Parser&, const Progress&) const
{
    return Match( data );
}

PossibleMatch Literal::Parse(const std::string_view data, const Parser&, const Progress&) const
{
    if (data.substr(0,literal.size()).compare(literal)==0)
        return Match( data.substr(literal.size()) );
    else
        return PossibleMatch();
}


PossibleMatch RegularExpression::Parse(const std::string_view data, const Parser&, const Progress&) const
{

    std::wstring wdata = from_utf8(data);
    std::wstring wexpression = from_utf8(expression);
    std::wregex  wexpr(wexpression);
    std::wsmatch match;
    if (std::regex_search(wdata, match, wexpr, std::regex_constants::match_continuous))
    {
        std::string match_str = to_utf8(match.str(0));
        return Match(data.substr(match_str.size()));
    }

    return PossibleMatch();
}

Whitespace::Whitespace(size_t amt, const std::string_view chars) :
    amount(amt),
    characters(chars)
{
}


PossibleMatch Whitespace::Parse(const std::string_view data, const Parser&, const Progress&) const
{
    if ((data.empty()) && (amount))
        return PossibleMatch();
    auto p = data.find_first_not_of(characters);
    if (p == data.npos)
        return Match{ data.substr(data.size()) };
    else if (p>=amount)
        return Match{ data.substr(p) };
    else
        return PossibleMatch();
}

PossibleMatch Disjunction::Parse(const std::string_view data, const Parser& parser , const Progress& progress) const
{
    for (const auto& e : expressions)
    {
        auto m = e->Parse(data, parser, progress);
        if (m)
            return m;
    }
    return PossibleMatch();
}

PossibleMatch Sequence::Parse(const std::string_view data, const Parser& parser , const Progress& progress) const
{
    auto remaining = data;
    std::vector<std::any> tokens;
    Match::Rules lastRules;
    for (const auto& e : expressions)
    {
        auto m = e->Parse(remaining, parser, progress);
        if (!m)
            return PossibleMatch();
        if (m->tokens.has_value())
        {
            // flatten vectors of any 
            if (m->tokens.type() == typeid(std::vector<std::any>))
            {
                auto mergeVector = std::any_cast<std::vector<std::any>>(m->tokens);
                tokens.insert(tokens.end(), mergeVector.begin(), mergeVector.end());
            }
            else
            {
                tokens.push_back(m->tokens);
            }
        }
                
        remaining = m->remaining;
        if (!m->rules.empty())
            lastRules = m->rules;
    }
    if (tokens.empty())
        return Match{ remaining };
    else if (tokens.size() == 1)
        return Match{ remaining, tokens.front(), lastRules };
    else
        return Match{ remaining, tokens, lastRules };
}

PossibleMatch Loop::Parse(const std::string_view data, const Parser& parser , const Progress& progress) const
{
    auto remaining = data;
    std::any tokens;
    Match::Rules lastRules;
    int index = 0;
    while (!remaining.empty())
    {
        auto m = expression->Parse(remaining, parser, progress);
        if (!m)
            break;
        if (m->remaining == remaining)
            break;

        auto len = m->remaining.data() - remaining.data();
        auto parsed = remaining.substr(0,len);
        std::string indexKey = std::string("[") + std::to_string(index++) + "]";
        tokens = parser.Merge(tokens, parser.Parse(indexKey, m->tokens, parsed));
        remaining = m->remaining;
        if (!m->rules.empty())
            lastRules = m->rules;
    }
    return Match{ remaining, tokens, lastRules };
}

Rule::Rule(const std::string_view n, const Expression& e) :
    name(n),
    expression(e)
{
    Declare::Define(*this);
}

Recursive::Recursive(const std::string_view n, const Expression& e) :
    Rule(n, e)
{
    recursive = true;
}

PossibleMatch Rule::Parse(const std::string_view data, const Parser& parser , const Progress& progress) const
{
    Progress newProgress = progress;
    if (!recursive)
    {
        // infinite recursion protection
        auto previousProgress = progress.find(this);
        if (previousProgress != progress.end() && previousProgress->second == data.size())
            return PossibleMatch();
        newProgress[this] = data.size();
    }
    auto m = expression->Parse(data, parser, newProgress);
    if (!m)
        return m;
    else
    {
        auto len = m->remaining.data() - data.data();
        std::string_view parsed = data.substr(0, len);
        auto newRules = m->rules;
        newRules.push_back(this);

        return Match{ m->remaining, parser.Parse(name, m->tokens, parsed), newRules };
    }
}

Declare::Declare(const std::string_view rule) :
    rule(rule)
{
}

std::map<const std::string_view, const Rule*> Declare::definitions;

void Declare::Define(const Rule& rule)
{
    definitions.insert(std::make_pair(rule.name, &rule));
}

const Rule& Declare::Get() const
{
    auto it = definitions.find(rule);
    if (it == definitions.end())
        throw std::runtime_error(std::string("Undefined rule ") + std::string(rule));
    return *it->second;
}


PossibleMatch Declare::Parse(const std::string_view data, const Parser& parser , const Progress& progress) const
{
    auto& rule = Get();
    return rule.Parse(data, parser, progress);
}


PossibleMatch Ref::Parse(const std::string_view data, const Parser& parser , const Progress& progress) const
{
    return rule->Parse(data, parser, progress);
}

Match Parse(const Rule& root, const Parser& parser , const std::string_view data)
{
    Progress start;
    auto possible = root.Parse(data, parser, start);
    if (!possible)
        throw SyntaxError(std::string("Couldn't match ") + std::string(root.name) +" at:" + std::string(data));
    else
        return *possible;
}

}
