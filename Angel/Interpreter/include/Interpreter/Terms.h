#pragma once 

#include "Interpreter/Symbol.h"
#include "Interpreter/SourceSpan.h"
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

class Literal
{
public:
    Literal(const std::string_view match) ;
    std::size_t Match(SourceSpan src) const;
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
    Regex(const Regex& other);
    Regex(Regex&& other);
    Regex& operator=(const Regex& other);
    Regex& operator=(Regex&& other);
    std::size_t Match(SourceSpan src) const;
    operator std::string() const;
    bool operator==(const Regex& other) const;
    bool operator!=(const Regex& other) const { return !this->operator==(other); }
private:
    friend struct std::hash<Interpreter::Regex>;
    std::wstring wmatch;
    std::unique_ptr<std::wregex> ConstructExpression() const;
    // expression is a unique pointer to reduce size of the Terms variant from 128 to 48
    std::unique_ptr<std::wregex> expression;
};

class Epsilon
{
public:
    Epsilon() = default;
    Epsilon(const Symbol& symbol);
    std::size_t Match(SourceSpan src) const;
    operator std::string() const;
    bool operator==(const Epsilon& other) const;
    bool operator!=(const Epsilon& other) const { return !this->operator==(other); }
    const Symbol& GetSymbol() const;
private:
    friend struct std::hash<Interpreter::Epsilon>;
    Symbol symbol;
};

using Token = std::variant<std::monostate, Literal, Regex, Epsilon>; 
using Term = std::variant<Literal, Regex, Epsilon, Symbol>;

template<typename T>
concept is_token = std::is_constructible_v<Token, T>;  // instead if is_same_v<T, Literal> || etc
template<typename T>
concept is_term = std::is_constructible_v<Term, T>; 

std::size_t Match(const Term& term, SourceSpan src);

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
struct hash<Interpreter::Epsilon>
{
    size_t operator()(const Interpreter::Epsilon& e) noexcept ;
};

string to_string(const Interpreter::Term& term);
}