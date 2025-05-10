#include "Interpreter/GrammarGenerator.h"
#include "Interpreter/Error.h"
#include "Interpreter/Utils.h"

namespace Interpreter
{


Terms GenerateTerms(Source& source, SymbolStream& parse)
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
        else if (input.symbol == Symbol("literal"))
        {
            std::string literal = Unclose(input.location.extract(source), '"');
            if (literal.empty())
                result.emplace_back(Epsilon()); // TODO named epsilon
            else   
                result.emplace_back(Literal(literal));
        }
        else if (input.symbol == Symbol("regex"))
        {
            std::string expression = Unclose(input.location.extract(source), '\'');
            if (expression.empty())
                result.emplace_back(Epsilon()); // TODO named epsilon
            else
                result.emplace_back(Regex(expression));
        }
        else if (input.symbol == Symbol("list-end"))
        {
            break;
        }
    }
    return result;

}   

Syntax GenerateGrammar(Source& source, SymbolStream& parse)
{
    source.clear();
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