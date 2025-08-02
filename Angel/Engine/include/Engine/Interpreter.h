#pragma once 

#include "Logic/Knowledge.h"
#include "Parser/Syntax.h"
#include "Parser/RecursiveDescentParser.h"
#include "Parser/Source.h"

namespace Angel::Engine
{

class AngelInterpreter
{
public:
    AngelInterpreter();
    std::size_t Interpret(const ::Parser::Source& source, Logic::Knowledge& knowledge ); 
    Logic::Expression InterpretQuery(const ::Parser::Source& source);
private:
    Parser::Syntax syntax;
    Parser::RecursiveDescentParser parser;

};


}