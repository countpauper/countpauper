#pragma once 
#include <string>
#include <variant> 

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
    std::string match;
};


}