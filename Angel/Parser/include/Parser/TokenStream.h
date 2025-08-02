#pragma once 

#include "Parser/SourceSpan.h"
#include "Parser/ObjectStream.h"
#include "Parser/Terms.h"

namespace Parser 
{

using hash_t=std::size_t;

struct InputToken 
{
    InputToken();
    explicit InputToken(hash_t token, SourceSpan ref);

    template<typename T>
    explicit InputToken(const T& token, SourceSpan ref) :
        InputToken(std::hash<T>()(token), ref)
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
