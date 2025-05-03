

#include "Interpreter/Parser.h"
#include "Interpreter/Syntax.h"
#include "Interpreter/Lexer.h"

namespace Interpreter 
{
    

class SymbolStringGenerator
{   // TODO: base generator interface or concept to iterate? 
    // TODO: pass source, This output will show it as symbol{source range}? 
    // TODO: hide hash optimization. lexicon shouldn't be needed here. 
    //   In release Symbols *are* hash (original name is lost)
    //   in debug symbols are string, no hashing needed 
    //   With that maybe symbols can be taken out of the lexicon again 
public:
    SymbolStringGenerator(Lexicon& lexicon, SymbolStream& stream) :
        lexicon(lexicon),
        stream(stream)
    {
    }

    std::string operator()() const
    {
        std::string result;
        for(auto symbol : stream.View())
        {
            if (symbol.location.length == 0)
                continue;
            if (!result.empty())
                result += " ";
            result += std::to_string(*lexicon[symbol.symbol]);
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

std::string Parser::ParseIt(const std::string_view source)
{
    TokenStream tokens;
    std::stringstream sourcestream;
    sourcestream << source;
    Lexicon lexicon(syntax);
    Lexer(lexicon).Process(sourcestream, tokens);
    SymbolStream os;
    Parse(tokens, os);
    return SymbolStringGenerator(lexicon, os)();
}

}