#pragma once 

#include "Parser/TokenStream.h"
#include "Parser/SymbolStream.h"
#include "Parser/Lexicon.h"
#include <string>

namespace Parser 
{

class Syntax;

class Parser 
{
public:
    virtual ~Parser() = default;
    virtual void ParseTokens(TokenStream& is, SymbolStream& os, const Symbol& start) = 0;
    void Parse(const Source& src, SymbolStream& os, const std::string_view start="");
    Symbols ParseIt(const Source& src, const std::string_view start="");
protected:
    Parser(const Syntax& syntax);
    const Syntax& syntax;
    Lexicon lexicon;
};

class LLParser : public Parser 
{
public:
    LLParser(const class Syntax& syntax, unsigned k);
    void ParseTokens(TokenStream& is, SymbolStream& os, const Symbol& start) override;
private:
};

}