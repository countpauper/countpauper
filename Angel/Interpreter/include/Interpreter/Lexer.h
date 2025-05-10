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
    Lexer(const class Lexicon& lexicon, std::size_t buffer=1024);
    void Process(Source& src, TokenStream& os);
    std::deque<InputToken> Process(const std::string_view input);
public: 
    void FillBuffer(std::istream& is);
    Lexicon lexicon;
    std::size_t buffer_size;
    std::string buffer;
};

};
