#pragma once 
#include <string>
#include <regex>
#include <variant>

namespace Interpreter 
{

using hash_t = std::size_t;

// Use std::variant only for terms.
// use concepts for the baes interface 
// see https://www.cppstories.com/2020/04/variant-virtual-polymorphism.html/
// How to refer to in 

/*
class Token
{
public:
    virtual bool IsEpsilon() const = 0;
    virtual std::size_t Match(const std::string_view input) const = 0;
};
*/

/*
template<typename T> 
std::size_t Match(const std::string_view input);

template<typename T>
concept Tokenize = requires(T a)
{
    { Match<T>{}(a) } -> std::convertible_to<std::size_t>;
};
*/

class Literal
{
public:
    Literal(const std::string_view match) ;
    bool IsEpsilon() const;
    std::size_t Match(const std::string_view input) const;
    bool operator==(const Literal& other) const;
    bool operator!=(const Literal& other) const { return !this->operator==(other); }
    operator std::string() const;
private:
    friend struct std::hash<Interpreter::Literal>;
    std::string match;
};


class Regex
{
public:
    Regex(const std::string_view match);
    bool IsEpsilon() const;
    std::size_t Match(const std::string_view input) const;
    operator std::string() const;
    bool operator==(const Regex& other) const;
    bool operator!=(const Regex& other) const { return !this->operator==(other); }
private:
    friend struct std::hash<Interpreter::Regex>;
    std::string match;
    std::regex expression;
};

struct Symbol 
{
    std::string name;
    operator std::string() const;
    std::size_t Match(const std::string_view input) const;
};

static const Literal epsilon("");

using Token = std::variant<Literal, Regex>; 
using Term = std::variant<Literal, Regex, Symbol>;

std::size_t TokenMatch(const Term& token, const std::string_view input);

}

template <>
struct std::hash<Interpreter::Literal>
{
    std::size_t operator()(const Interpreter::Literal& l) noexcept ;
};

template <>
struct std::hash<Interpreter::Regex>
{
    std::size_t operator()(const Interpreter::Regex& l) noexcept ;
};


template <>
struct std::hash<Interpreter::Symbol>
{
    std::size_t operator()(const Interpreter::Symbol& s) noexcept ;
};