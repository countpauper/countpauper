#include "Interpreter/GrammarGenerator.h"
#include "Interpreter/Error.h"
#include "Interpreter/Utils.h"

namespace Interpreter
{


Terms GenerateTerms(std::istream& source, SymbolStream& parse)
{
    Terms result;
    ParsedSymbol input;
    while(!parse.eof())
    {
        parse >> input;
        if (input.symbol == Symbol("rule-name"))
        {   
            result.emplace_back(Symbol(Unclose(input.location.extract(source), '<','>')));
        }
        else if (input.symbol == Symbol("Literal"))
        {
            result.emplace_back(Literal(Unclose(input.location.extract(source), '"')));
        }
        else if (input.symbol == Symbol("Regex"))
        {
            result.emplace_back(Regex(Unclose(input.location.extract(source), '\'')));
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
            name = Symbol(Unclose(input.location.extract(source), '<', '>'));
        }
        else if (input.symbol == Symbol("list"))
        {
            result.emplace(name, GenerateTerms(source, parse));
        }
    }
    return std::move(result);
 }

}