#pragma once 

#include <cstddef>


namespace Interpreter 
{
    struct SourceSpan {
        size_t from;
        size_t length;
        bool operator==(const SourceSpan& o) const { return from == o.from && length == o.length; }
    };    
}