#include "Interpreter/GrammarGenerator.h"
#include "Interpreter/Error.h"

namespace Interpreter
{

std::string Extract(std::istream& source, SourceSpan location)
{
    std::string result(location.length, '?');
    source.seekg(location.from);
    source.read(result.data(), location.length);
    return result;
}

Terms GenerateTerms(std::istream& source, SymbolStream& parse)
{
    Terms result;
    ParsedSymbol input;
    while(!parse.eof())
    {
        parse >> input;
        if (input.symbol == Symbol("rule-name"))
        {   
            result.emplace_back(Symbol(Extract(source, input.location.sub(1,-1))));
        }
        else if (input.symbol == Symbol("Literal"))
        {
            result.emplace_back(Literal(Extract(source, input.location.sub(1,-1))));
        }
        else if (input.symbol == Symbol("Regex"))
        {
            result.emplace_back(Regex(Extract(source, input.location.sub(1,-1))));
        }
        else if (input.symbol == Symbol("list-end"))
        {
            break;
        }
    }
    return result;

}   

Syntax GrammarGenerator::operator()(std::istream& source, SymbolStream& parse) const
{
    source.exceptions(source.badbit|source.failbit|source.eofbit);

    Syntax result;
    Symbol name;
    ParsedSymbol input; 
    while(!parse.eof())
    {
        parse >> input;
        if (input.symbol == Symbol("rule-name"))
        {
            name = Symbol(Extract(source, input.location));
        }
        else if (input.symbol == Symbol("list"))
        {
            result.emplace(name, GenerateTerms(source, parse));
        }
    }
    return std::move(result);
 }

}