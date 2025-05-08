#pragma once 
#include <iostream>
#include "Interpreter/SymbolStream.h"
#include "Interpreter/Syntax.h"
#include "Interpreter/Source.h"

namespace Interpreter 
{
class GrammarGenerator 
{
public:
    Syntax operator()(Source& source, SymbolStream& parse) const;
};

}
