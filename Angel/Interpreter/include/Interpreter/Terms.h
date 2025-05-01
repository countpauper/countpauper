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
    bool operator==(const Symbol& other) const;
    bool operator!=(const Symbol& other) const { return !this->operator==(other); }
};

static const Literal epsilon("");

using Token = std::variant<std::monostate, Literal, Regex>; 
using Term = std::variant<Literal, Regex, Symbol>;

template<typename T>
concept is_token = std::is_constructible_v<Token, T>;  // instead if is_same_v<T, Literal> || etc
template<typename T>
concept is_term = std::is_constructible_v<Term, T>; 

std::size_t Match(const Term& term, const std::string_view input);
bool IsEpsilon(const Token& token);

}

namespace std
{
template <>
struct hash<Interpreter::Literal>
{
    size_t operator()(const Interpreter::Literal& l) noexcept ;
};

template <>
struct hash<Interpreter::Regex>
{
    size_t operator()(const Interpreter::Regex& l) noexcept ;
};


template <>
struct hash<Interpreter::Symbol>
{
    size_t operator()(const Interpreter::Symbol& s) noexcept ;
};

string to_string(const Interpreter::Term& term);
}