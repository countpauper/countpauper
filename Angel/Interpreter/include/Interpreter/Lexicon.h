#pragma once 
#include <unordered_set>

namespace Interpreter 
{

class Token;

class Lexicon : public std::unordered_set<const Token*>
{
public:
    explicit Lexicon(std::initializer_list<const Token*> tokens);
    explicit Lexicon(const class Syntax& syntax);
};

}