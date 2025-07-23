#include "Interpreter/Lexer.h"
#include "Interpreter/Error.h"
#include "Interpreter/Terms.h"
#include <sstream>

namespace Interpreter 
{

Lexer::Lexer(const Lexicon& lexicon) :
    lexicon(lexicon)
{
}

void Lexer::Process(const Source& src, TokenStream& os)
{
    std::size_t location = 0;
    while(location<src.size())
    {
        hash_t best = 0;
        std::size_t bestConsumed = 0;
        for(auto tokenPair : lexicon)
        {
            if (!tokenPair.second)
                continue;
            auto consumed = Match(*tokenPair.second, src.span(location));
            if (consumed>bestConsumed) {
                best = tokenPair.first;
                bestConsumed = consumed;
            }
        }
        if (bestConsumed == 0) 
        {
            throw Error("Unknown token", src.span(location, src.size()-location));
        }
        os << InputToken(best, src.span(location, bestConsumed));
        location += bestConsumed;
    }
    os << InputToken(0, SourceSpan{location, src.size()-location, &src}); // end token
}
    
std::deque<InputToken> Lexer::Process(const Source& src)
{
    TokenStream os;
    Process(src, os);
    return os.Dump();   // must return a copy and no a view, stream is going out of scope
}

}
