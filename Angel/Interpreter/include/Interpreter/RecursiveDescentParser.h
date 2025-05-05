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
    void Parse(TokenStream& is, SymbolStream& os) override;
private:
    using InputIterator = std::deque<InputToken>::iterator;
    std::vector<OutputSymbol> Recurse(Symbol rule, 
        InputIterator& from,InputIterator to);
    std::vector<OutputSymbol> Recurse(Symbol symbol, const Terms& terms, 
        InputIterator& from, InputIterator to);
    };

}
