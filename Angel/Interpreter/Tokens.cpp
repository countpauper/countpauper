#include "Interpreter/Tokens.h"
#include <regex>

namespace Interpreter
{

Literal::Literal(const std::string_view match):
    match(match)
{
}

bool Literal::IsEpsilon() const
{
    return match.empty();
}

std::size_t Literal::Match(const std::string_view input) const
{
    if (input.starts_with(match))
        return match.size();
    else
        return 0;
}

Literal::operator std::string() const
{
    return std::format("\"{}\"", match.c_str());
}

bool Literal::operator==(const Literal& other) const
{
    return other.match == match;
}

Regex::Regex(const std::string_view match):
    match(match),
    expression(match.data(), match.size(), 
        std::regex_constants::ECMAScript | 
        std::regex_constants::optimize | 
        std::regex_constants::nosubs)
{
}

bool Regex::IsEpsilon() const
{
    return match.empty();
}

std::size_t Regex::Match(const std::string_view input) const
{
    std::cmatch result;
    if (std::regex_search(input.begin(), input.end(), result, expression, 
            std::regex_constants::match_continuous | std::regex_constants::match_not_null))
        return result.length();
    else 
        return 0;
}

Regex::operator std::string() const
{
    return std::format("'{}'", match.c_str());
}

bool Regex::operator==(const Regex& other) const
{
    return other.match == match;
}

std::size_t Symbol::Match(const std::string_view) const 
{ 
    return 0; 
}

std::size_t TokenMatch(const Term& token, const std::string_view input)
{
    auto matcher = [&input](const auto& obj) { return obj.Match(input); };
    return std::visit(matcher, token);
}

}

namespace std
{


size_t hash<Interpreter::Literal>::operator()(const Interpreter::Literal& l) noexcept 
{
    std::hash<std::string> hasher;
    return typeid(Interpreter::Literal).hash_code() ^ hasher(l.match);
}

size_t hash<Interpreter::Regex>::operator()(const Interpreter::Regex& r) noexcept 
{
    std::hash<std::string> hasher;
    return typeid(Interpreter::Regex).hash_code() ^ hasher(r.match);
}


size_t hash<Interpreter::Symbol>::operator()(const Interpreter::Symbol& s) noexcept 
{
    std::hash<std::string> hasher;
    return typeid(Interpreter::Symbol).hash_code() ^ hasher(s.name);
}
}