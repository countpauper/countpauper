#include "Interpreter/TokenStream.h"

namespace Interpreter 
{

TokenStream& TokenStream::operator<<(const InputToken& token)
{
    tokens.push_back(token);
    return *this;
}

TokenStream& TokenStream::operator>>(InputToken& token)
{
    token = tokens[0];
    tokens.pop_front();
    return *this;
}

std::deque<InputToken> TokenStream::Flush()
{
    std::deque<InputToken> result;
    std::swap(result, tokens);
    return result;
}


}
