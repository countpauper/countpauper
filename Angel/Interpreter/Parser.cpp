

#include "Interpreter/Parser.h"
#include "Interpreter/Syntax.h"
#include "Interpreter/Lexer.h"
#include "Interpreter/Error.h"

namespace Interpreter 
{
    

class SymbolGenerator
{  
public:
    SymbolGenerator(Lexicon& lexicon, SymbolStream& stream) :
        lexicon(lexicon),
        stream(stream)
    {
    }

    Symbols operator()() const
    {
        Symbols result;
        for(auto element : stream.View())
        {
            if (!element.symbol)
                continue;
            if (element.symbol == Symbol("opt-whitespace"))
                continue;
            result.push_back(element.symbol);
        }
        return result;
    }
private:
    Lexicon& lexicon;
    SymbolStream& stream;
};

Parser::Parser(const Syntax& syntax) :
    syntax(syntax),
    lexicon(syntax)
{
}

void CheckTrailing(const Source& src, TokenStream& tokens)
{
    InputToken trailing;
    tokens >> trailing;
    if (!tokens.eof())
    {
        throw Error("Trailing tokens", trailing.reference);
    }
}

void Parser::Parse(const Source& src, SymbolStream& os, const std::string_view start)
{
    TokenStream tokens;
    Lexer(lexicon).Process(src, tokens);
    Symbol startSymbol;
    if (start.empty())
        startSymbol = syntax.Root();
    else
        startSymbol = Symbol(start);

    ParseTokens(tokens, os, startSymbol);
    CheckTrailing(src, tokens);
}


Symbols Parser::ParseIt(const Source& source, const std::string_view start)
{
    SymbolStream os;
    Parse(source, os, start);
    return SymbolGenerator(lexicon, os)();
}

}