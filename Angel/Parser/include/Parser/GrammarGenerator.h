#pragma once 
#include <iostream>
#include "Parser/SymbolStream.h"
#include "Parser/Syntax.h"
#include "Parser/Source.h"

namespace Parser 
{
    Syntax GenerateGrammar(const Source& source, SymbolStream& parse);
}
