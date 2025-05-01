#include "Interpreter/Lexer.h"
#include "Interpreter/Error.h"
#include "Interpreter/Terms.h"
#include <sstream>

namespace Interpreter 
{

Lexer::Lexer(const Lexicon& lexicon, std::size_t buffer) :
    lexicon(lexicon),
    buffer_size(buffer)
{
}

void Lexer::FillBuffer(std::istream& is)
{
    auto pos = buffer.size();
    auto missing = buffer_size - pos - 1;
    buffer.append("\x0", missing);
    is.read(buffer.data() + pos, missing);
    auto read = is.gcount();
    buffer.resize(pos + read); 
}

void Lexer::Process(std::istream& is, TokenStream& os)
{
    std::size_t location = 0;
    while(true)
    {
        FillBuffer(is);
        if (buffer.empty())
            break;
        hash_t best = 0;
        std::size_t bestConsumed = 0;
        for(auto tokenPair : lexicon)
        {
            auto consumed = Match(*tokenPair.second, buffer);
            if (consumed>bestConsumed) {
                best = tokenPair.first;
                bestConsumed = consumed;
            }
        }
        if (bestConsumed == 0) 
        {
            throw Error("Unkown token", SourceSpan{location, buffer.size()});
        }
        os << InputToken(best, location, bestConsumed);
        buffer.erase(0, bestConsumed);
        location += bestConsumed;
    }
    os << InputToken(0, location, 0); // end token
}
    
std::deque<InputToken> Lexer::Process(const std::string_view input)
{
    std::stringstream is;
    is << input;     // C++ 26 can take a string_view as constructor argument
    TokenStream os;
    Process(is, os);
    return os.Flush();
}

}
