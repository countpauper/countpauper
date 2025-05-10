#pragma once 

#include "Interpreter/TokenStream.h"
#include "Interpreter/SymbolStream.h"
#include "Interpreter/Lexicon.h"
#include <string>

namespace Interpreter 
{

class Syntax;

class Parser 
{
public:
    virtual ~Parser() = default;
    virtual void ParseTokens(TokenStream& is, SymbolStream& os) = 0;
    void Parse(Source& src, SymbolStream& os);
    Symbols ParseIt(const std::string_view source);
protected:
    Parser(const Syntax& syntax);
    const Syntax& syntax;
    Lexicon lexicon;
};

class LLParser : public Parser 
{
public:
    LLParser(const class Syntax& syntax, unsigned k);
    void ParseTokens(TokenStream& is, SymbolStream& os) override;
private:
};

}