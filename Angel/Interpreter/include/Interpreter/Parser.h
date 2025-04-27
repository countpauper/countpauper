#pragma once 

#include "Interpreter/SourceSpan.h"
#include "Interpreter/ObjectStream.h"
#include <string>
namespace Interpreter 
{

struct OutputSymbol
{
    std::string_view symbol;
    SourceSpan location;
};

using SymbolStream = InfiniteObjectStream<OutputSymbol>; 

class Parser 
{
public:
    virtual ~Parser() = default;
    virtual void Parse(class TokenStream& is, SymbolStream& os) = 0;
};

class LLParser : public Parser 
{
public:
    LLParser(const class Syntax& syntax, unsigned k);
    void Parse(class TokenStream& is, SymbolStream& os) override;
private:
};

}