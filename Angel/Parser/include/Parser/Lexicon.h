#pragma once 

#include <unordered_set>
#include "Terms.h"

namespace Parser 
{
using hash_t=std::size_t;

class Lexicon : public std::map<hash_t, const Term*>
{
public:
    explicit Lexicon(std::initializer_list<const Term*> tokens);
    explicit Lexicon(const class Syntax& syntax);
private:

};

}