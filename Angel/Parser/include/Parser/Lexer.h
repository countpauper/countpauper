#pragma once 

#include "Parser/TokenStream.h"
#include "Parser/Lexicon.h"
#include "Parser/Source.h"
#include <deque>
#include <istream>

namespace Parser 
{

class Lexer
{
public:
    Lexer(const class Lexicon& lexicon);
    void Process(const Source& src, TokenStream& os);
    std::deque<InputToken> Process(const Source& src);
public: 
    Lexicon lexicon;
};

};
