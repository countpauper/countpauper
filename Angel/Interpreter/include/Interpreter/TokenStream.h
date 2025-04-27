#pragma once 

#include "interpreter/SourceSpan.h"
#include "Interpreter/ObjectStream.h"

namespace Interpreter 
{
class Token;

struct InputToken {
    const Token* token;
    SourceSpan reference; 
    bool operator==(const InputToken& other) const { return token == other.token && reference == other.reference; }
};

using TokenStream = InfiniteObjectStream<InputToken>;

};
