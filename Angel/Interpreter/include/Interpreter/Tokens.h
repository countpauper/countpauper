#pragma once 
#include <string>
#include <variant> 
#include <regex>

namespace Interpreter 
{

using hash_t = std::size_t;

class Token
{
public:
    virtual bool IsEpsilon() const = 0;
    virtual std::size_t Match(const std::string_view input) const = 0;
    virtual hash_t Hash() const = 0;
};

class Literal : public Token
{
public:
    Literal(std::string_view match) ;
    bool IsEpsilon() const;
    std::size_t Match(const std::string_view input) const;
    bool operator==(const Token& other) const;
    bool operator!=(const Token& other) const { return !this->operator==(other); }
    operator std::string() const;
    hash_t Hash() const override;
private:
    std::string match;
};

class Regex : public Token
{
public:
    Regex(std::string_view match);
    bool IsEpsilon() const;
    std::size_t Match(const std::string_view input) const;
    operator std::string() const;
    bool operator==(const Token& other) const;
    bool operator!=(const Token& other) const { return !this->operator==(other); }
    hash_t Hash() const override;
private:
    std::string match;
    std::regex expression;
};

static const Literal epsilon("");

}