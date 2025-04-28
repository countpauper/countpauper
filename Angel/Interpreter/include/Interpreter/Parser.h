#pragma once 

#include "Interpreter/SourceSpan.h"
#include "Interpreter/ObjectStream.h"
#include "Interpreter/TokenStream.h"
#include <string>
#include <vector>

namespace Interpreter 
{

class Syntax;

struct OutputSymbol
{
    const std::string_view symbol;
    SourceSpan location;
};

using SymbolStream = InfiniteObjectStream<OutputSymbol>; 

class Parser 
{
public:
    virtual ~Parser() = default;
    virtual void Parse(TokenStream& is, SymbolStream& os) = 0;
    std::vector<std::string> ParseIt(const std::string_view source);
protected:
    Parser(const Syntax& syntax);
    const Syntax& syntax;
};

class LLParser : public Parser 
{
public:
    LLParser(const class Syntax& syntax, unsigned k);
    void Parse(TokenStream& is, SymbolStream& os) override;
private:
};

}