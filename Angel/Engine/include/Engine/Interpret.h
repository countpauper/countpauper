#pragma once 

#include "Logic/Knowledge.h"
#include "Logic/Predicate.h"
#include "Interpreter/Source.h"

namespace Angel::Engine
{

std::vector<Logic::Predicate> Interpret(Interpreter::Source& source, Logic::Knowledge& knowledge ); 

}