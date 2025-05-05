#pragma once 
#include <iostream>
#include "Interpreter/SymbolStream.h"
#include "Interpreter/Syntax.h"

namespace Interpreter 
{
class GrammarGenerator 
{
public:
    Syntax operator()(std::istream& source, SymbolStream& parse) const;
};

}
