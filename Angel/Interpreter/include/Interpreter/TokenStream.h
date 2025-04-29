#pragma once 

#include "interpreter/SourceSpan.h"
#include "Interpreter/ObjectStream.h"

namespace Interpreter 
{

using hash_t=std::size_t;

struct InputToken 
{
    InputToken();
    explicit InputToken(hash_t token, size_t from, size_t length);
    explicit InputToken(const class Token& token, size_t from, size_t length);
    hash_t token;
    SourceSpan reference; 
    bool operator==(const InputToken& other) const { return token == other.token && reference == other.reference; }
};

using TokenStream = InfiniteObjectStream<InputToken>;

};
