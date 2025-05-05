#pragma once 

#include "Interpreter/TokenStream.h"
#include "Interpreter/SymbolStream.h"
#include <string>

namespace Interpreter 
{

class Syntax;

class Parser 
{
public:
    virtual ~Parser() = default;
    virtual void Parse(TokenStream& is, SymbolStream& os) = 0;
    Symbols ParseIt(const std::string_view source);
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