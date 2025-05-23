#include "Interpreter/Terms.h"
#include "Interpreter/Utils.h"
#include "Interpreter/Source.h"
#include <regex>

namespace Interpreter
{

Literal::Literal(const std::string_view match):
    match(match)
{
    if (match.empty())
        throw std::invalid_argument("Literal token can not be empty. Use Epsilon.");
}

std::size_t Literal::Match(SourceSpan src) const
{
    std::string srcStrart = src.sub(0, match.size()).extract();

    if (match == srcStrart)
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
    if (match.empty())
        throw std::invalid_argument("Regex token can not be empty. Use Epsilon.");
}

std::size_t Regex::Match(SourceSpan src) const
{
    std::cmatch result;
    std::string srcString = src.extract(); // TODO optimize with iterator, this copies a lot of source

    if (std::regex_search(srcString.c_str(), result, expression, 
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

std::size_t Match(const Term& token, SourceSpan src)
{
    auto matcher = [&src](const auto& obj) { return obj.Match(src); };
    return std::visit(matcher, token);
}

Epsilon::Epsilon(const Symbol& symbol) :
    symbol(symbol)
{
}

const Symbol& Epsilon::GetSymbol() const
{
    return symbol;
}

std::size_t Epsilon::Match(SourceSpan) const
{
    return 0;
}

Epsilon::operator std::string() const
{
    if (symbol && symbol!=Symbol::epsilon)
        return std::format("ε{}", std::string(symbol));
    else
        return "ε";
}

bool Epsilon::operator==(const Epsilon& other) const
{
    return other.symbol == symbol;
}


}

namespace std
{


size_t hash<Interpreter::Literal>::operator()(const Interpreter::Literal& l) noexcept 
{
    std::hash<std::string> hasher;
    return hasher(l.match);
}

size_t hash<Interpreter::Regex>::operator()(const Interpreter::Regex& r) noexcept 
{
    std::hash<std::string> hasher;
    return hasher(r.match);
}

size_t hash<Interpreter::Epsilon>::operator()(const Interpreter::Epsilon& e) noexcept 
{
    std::hash<std::string> hasher;
    return e.symbol.Hash();
}

string to_string(const Interpreter::Term& term)
{
    return visit(
        [](const auto& obj) { return string(obj); },
        term);
}

}