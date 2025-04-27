#pragma once 

#include "Interpreter/TokenStream.h"
#include <set>
#include <deque>

namespace Interpreter 
{

using Lexicon = std::set<const Token*>;

class Lexer
{
public:
    Lexer(const Lexicon& lexicon, std::size_t buffer=1024);
    void Process(std::istream& is, TokenStream& os);
    std::deque<InputToken> Process(const std::string_view input);
public: 
    void FillBuffer(std::istream& is);
    Lexicon lexicon;
    std::size_t buffer_size;
    std::string buffer;
};

};
