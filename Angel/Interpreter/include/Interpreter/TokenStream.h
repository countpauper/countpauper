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
    hash_t token;
    SourceSpan reference; 
    bool operator==(const InputToken& other) const { return token == other.token && reference == other.reference; }
};

using TokenStream = Fifo<InputToken>;

};
