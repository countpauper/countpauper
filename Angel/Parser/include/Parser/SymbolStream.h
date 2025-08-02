#pragma once 

#include "Parser/ObjectStream.h"
#include "Parser/SourceSpan.h"
#include "Parser/Terms.h"

namespace Parser 
{
    struct ParsedSymbol
    {
        Symbol symbol;
        SourceSpan location;
    };
    
    using SymbolStream = Fifo<ParsedSymbol>; 
    static_assert(StreamExtractableWithEof<SymbolStream, ParsedSymbol>);    
}