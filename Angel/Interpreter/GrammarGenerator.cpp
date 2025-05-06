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

Syntax GrammarGenerator::operator()(std::istream& source, SymbolStream& parse) const
{
    source.exceptions(source.badbit|source.failbit|source.eofbit);

    Syntax result;
    Symbol name;
    Syntax::iterator rule = result.end(); 
    for(const auto& symbol : parse.View())
    {
        if (symbol.symbol == Symbol("rule-name"))
        {
            if (rule==result.end())
            {
                name = Symbol(Extract(source, symbol.location));
            }
            else
            {
                rule->terms.emplace_back(Symbol(Extract(source, symbol.location)));
            }
        }
        else if (symbol.symbol == Symbol("list"))
        {
            rule = result.emplace(name, Terms());
        }
        else if (symbol.symbol == Symbol("list-end"))
        {
            rule = result.end();
        }
        else if (symbol.symbol == Symbol("Literal"))
        {
            rule->terms.emplace_back(Literal(Extract(source, symbol.location)));
        }
        else if (symbol.symbol == Symbol("Regex"))
        {
            rule->terms.emplace_back(Regex(Extract(source, symbol.location)));
        }
    }
    return std::move(result);
 }

}