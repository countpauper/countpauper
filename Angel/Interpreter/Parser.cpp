

#include "Interpreter/Parser.h"
#include "Interpreter/Syntax.h"
#include "Interpreter/Lexer.h"

namespace Interpreter 
{
    

class SymbolGenerator
{   // TODO: base generator interface or concept to iterate? 
    // TODO: pass source, This output will show it as symbol{source range}? 
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

void Parser::Parse(Source& src, SymbolStream& os)
{
    TokenStream tokens;
    Lexer(lexicon).Process(src, tokens);
    ParseTokens(tokens, os);
}


Symbols Parser::ParseIt(const std::string_view source)
{
    Source src(source);
    SymbolStream os;
    Parse(src, os);
    return SymbolGenerator(lexicon, os)();
}

}