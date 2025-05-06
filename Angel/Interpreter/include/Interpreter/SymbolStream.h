#pragma once 

#include "Interpreter/ObjectStream.h"
#include "Interpreter/SourceSpan.h"
#include "Interpreter/Terms.h"

namespace Interpreter 
{
    struct ParsedSymbol
    {
        Symbol symbol;
        SourceSpan location;
    };
    
    using SymbolStream = Fifo<ParsedSymbol>; 
    static_assert(StreamExtractableWithEof<SymbolStream, ParsedSymbol>);    
}