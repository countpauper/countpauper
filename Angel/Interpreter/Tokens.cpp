#include "Interpreter/Tokens.h"
#include <regex>

namespace Interpreter
{

Literal::Literal(std::string_view match):
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

bool Literal::operator==(const Token& other) const
{
    if (const Literal* o = dynamic_cast<const Literal*>(&other))
        return o->match == match;
    else
        return false;
}

Regex::Regex(std::string_view match):
    expression(match.data(), match.size(), 
        std::regex_constants::ECMAScript | 
        std::regex_constants::optimize | 
        std::regex_constants::nosubs)
{
}

bool Regex::IsEpsilon() const
{
    return false;
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

}