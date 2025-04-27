#pragma once 

#include <cstddef>


namespace Interpreter 
{
    struct SourceSpan {
        size_t from = 0;
        size_t length = 0;
        bool operator==(const SourceSpan& o) const { return from == o.from && length == o.length; }
    };    
}