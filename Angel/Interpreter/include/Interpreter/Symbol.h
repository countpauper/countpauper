#pragma once 

#include "Interpreter/Symbol.h"
#include <string>
#include <regex>
#include <variant>

namespace Interpreter 
{

using hash_t = std::size_t;

struct Symbol 
{
#ifdef NDEBUG
    hash_t label;
#else 
    std::string name;
#endif    
    Symbol();
    Symbol(const char* name);
    Symbol(const std::string_view name);
    operator std::string() const;
    hash_t Hash() const;
    std::size_t Match(const std::string_view input) const;
    bool operator<(const Symbol& other) const;
    operator bool() const;

    bool operator==(const Symbol& other) const;
    bool operator!=(const Symbol& other) const { return !this->operator==(other); }
};

using Symbols = std::vector<Symbol>;

}

namespace std
{
template <>
struct hash<Interpreter::Symbol>
{
    size_t operator()(const Interpreter::Symbol& s) noexcept ;
};

}