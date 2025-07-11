#pragma once 

#include "Interpreter/Parser.h"
#include "Interpreter/Syntax.h"
#include <vector> 

namespace Interpreter 
{

class RecursiveDescentParser : public Parser 
{
public:
    explicit RecursiveDescentParser(const Syntax& syntax);
    void ParseTokens(TokenStream& is, SymbolStream& os, const Symbol& start) override;
private:
    std::vector<ParsedSymbol> Recurse(const Symbol& rule, 
        TokenStream& is);
    std::vector<ParsedSymbol> Recurse(const Symbol& symbol, const Terms& terms, 
        TokenStream& is);
    };

}
