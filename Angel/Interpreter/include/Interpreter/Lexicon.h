#pragma once 

#include <unordered_set>
#include "Tokens.h"

namespace Interpreter 
{
using hash_t=std::size_t;

class Lexicon : public std::map<hash_t, const Token*>
{
public:
    explicit Lexicon(std::initializer_list<const Token*> tokens);
    explicit Lexicon(const class Syntax& syntax);
};

}