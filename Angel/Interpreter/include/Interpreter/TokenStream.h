#pragma once 

#include "interpreter/SourceSpan.h"
#include "Interpreter/ObjectStream.h"
#include "Interpreter/Terms.h"

namespace Interpreter 
{

using hash_t=std::size_t;

struct InputToken 
{
    InputToken();
    explicit InputToken(hash_t token, size_t from, size_t length);

    template<typename T>
    explicit InputToken(const T& token, size_t from, size_t length) :
        InputToken(std::hash<T>()(token), from, length)
    {
    }
    operator std::string() const;
    bool operator==(const InputToken& other) const;

    hash_t token;
    SourceSpan reference; 
};

using TokenStream = Fifo<InputToken>;
static_assert(StreamExtractableWithEof<TokenStream, InputToken>);
};
