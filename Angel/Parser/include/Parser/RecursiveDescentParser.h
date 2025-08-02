#pragma once 

#include "Parser/Parser.h"
#include "Parser/Syntax.h"
#include <vector> 

namespace Parser 
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
