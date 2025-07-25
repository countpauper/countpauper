#pragma once 

#include "Logic/Knowledge.h"
#include "Interpreter/Syntax.h"
#include "Interpreter/RecursiveDescentParser.h"
#include "Interpreter/Source.h"

namespace Angel::Engine
{

class AngelInterpreter
{
public:
    AngelInterpreter();
    std::size_t Interpret(const ::Interpreter::Source& source, Logic::Knowledge& knowledge ); 
    Logic::Expression InterpretQuery(const ::Interpreter::Source& source);
private:
    Interpreter::Syntax syntax;
    Interpreter::RecursiveDescentParser parser;

};


}