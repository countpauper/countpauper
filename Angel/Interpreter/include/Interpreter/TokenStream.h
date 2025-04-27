#pragma once 

#include "interpreter/SourceSpan.h"
#include <deque>

namespace Interpreter 
{
class Token;

struct InputToken {
    const Token* token;
    SourceSpan reference; 
    bool operator==(const InputToken& other) const { return token == other.token && reference == other.reference; }
};

class TokenStream {
public:
    TokenStream& operator<<(const InputToken& token);
    TokenStream& operator>>(InputToken& token);
    std::deque<InputToken> Flush(); 
private:
    std::deque<InputToken> tokens;
};

};
