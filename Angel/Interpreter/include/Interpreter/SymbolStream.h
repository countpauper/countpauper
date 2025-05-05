#pragma once 

#include "Interpreter/ObjectStream.h"
#include "Interpreter/SourceSpan.h"
#include "Interpreter/Terms.h"

namespace Interpreter 
{
    struct OutputSymbol
    {
        Symbol symbol;
        SourceSpan location;
    };
    
    using SymbolStream = Fifo<OutputSymbol>; 
    static_assert(StreamExtractableWithEof<SymbolStream, OutputSymbol>);    
}