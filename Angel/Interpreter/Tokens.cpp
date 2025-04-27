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


Regex::Regex(std::string_view match):
    match(match)
{
}

bool Regex::IsEpsilon() const
{
    return match.empty();
}

std::size_t Regex::Match(const std::string_view input) const
{
    std::regex expr(match);
    std::cmatch result;
    if (std::regex_search(input.begin(), input.end(), result, expr, std::regex_constants::match_continuous))
        return result.str(0).size();
    else 
        return 0;
}

}