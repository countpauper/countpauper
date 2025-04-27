#pragma once 
#include <string>
#include <variant> 
#include <regex>

namespace Interpreter 
{

class Token
{
public:
    virtual bool IsEpsilon() const = 0;
    virtual std::size_t Match(const std::string_view input) const = 0;
};

class Literal : public Token
{
public:
    Literal(std::string_view match) ;
    bool IsEpsilon() const;
    std::size_t Match(const std::string_view input) const;
    bool operator==(const Token& other) const;
    bool operator!=(const Token& other) const { return !this->operator==(other); }
private:
    std::string match;
};

class Regex : public Token
{
public:
    Regex(std::string_view match);
    bool IsEpsilon() const;
    std::size_t Match(const std::string_view input) const;
private:
    std::regex expression;
};

static const Literal epsilon("");

}