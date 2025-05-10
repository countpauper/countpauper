#pragma once 
#include <iostream>
#include "Interpreter/SymbolStream.h"
#include "Interpreter/Syntax.h"
#include "Interpreter/Source.h"

namespace Interpreter 
{
    Syntax GenerateGrammar(const Source& source, SymbolStream& parse);
}
