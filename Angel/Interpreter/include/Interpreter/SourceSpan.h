#pragma once 

#include <cstddef>
#include <iostream>

namespace Interpreter 
{
    class Source;

    struct SourceSpan 
    {
        size_t from = 0;
        size_t length = 0;
        const Source* source = nullptr;

        // Soure source = nullptr;
        // TODO: a Source itself should be a class that wraps source text (on disk or in memory)
        // and the SourceSpan should refer to that specific source as well as the span.
        // extract() should take from that specific source. 

        size_t size() const;
        bool empty() const;
        operator std::string() const;
        bool operator==(const SourceSpan& o) const;
        bool operator!=(const SourceSpan& o) const { return !(*this == o); }
        SourceSpan sub(std::ptrdiff_t offset, std::ptrdiff_t newLength=std::numeric_limits<std::ptrdiff_t>::max()) const;
        std::string extract() const;
        
    };    

    std::ostream& operator<<(std::ostream& os, const SourceSpan& span);
}