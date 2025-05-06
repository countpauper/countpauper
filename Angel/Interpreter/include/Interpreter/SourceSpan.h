#pragma once 

#include <cstddef>
#include <algorithm>

namespace Interpreter 
{
    struct SourceSpan {
        size_t from = 0;
        size_t length = 0;
        bool operator==(const SourceSpan& o) const { return from == o.from && length == o.length; }
        SourceSpan sub(long offset, long newLength) const 
        { 
            size_t newStart = std::max(0L, offset + static_cast<long>(from));
            long maxLength = (from+length - newStart);
            if (newLength<0)
                newLength = std::max(0L, static_cast<long>(length) + newLength -1);
            return SourceSpan(newStart, std::min(newLength, maxLength));
        }
    };    
}