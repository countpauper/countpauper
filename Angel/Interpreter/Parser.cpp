

#include "Interpreter/Parser.h"
#include "Interpreter/Syntax.h"
#include "Interpreter/Lexer.h"

namespace Interpreter 
{
    

class SymbolGenerator
{   // TODO: base generator interface or concept to iterate? 
    // TODO: pass source, This output will show it as symbol{source range}? 
    // TODO: hide hash optimization. lexicon shouldn't be needed here. 
    //   In release Symbols *are* hash (original name is lost)
    //   in debug symbols are string, no hashing needed 
    //   With that maybe symbols can be taken out of the lexicon again 
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
            if (element.location.length == 0)
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
    syntax(syntax)
{
}

Symbols Parser::ParseIt(const std::string_view source)
{
    TokenStream tokens;
    std::stringstream sourcestream;
    sourcestream << source;
    Lexicon lexicon(syntax);
    Lexer(lexicon).Process(sourcestream, tokens);
    SymbolStream os;
    Parse(tokens, os);
    return SymbolGenerator(lexicon, os)();
}

}