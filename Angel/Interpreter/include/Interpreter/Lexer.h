#pragma once 

#include "Interpreter/TokenStream.h"
#include "Interpreter/Lexicon.h"
#include "Interpreter/Source.h"
#include <deque>
#include <istream>

namespace Interpreter 
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
