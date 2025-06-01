#include "Logic/Trace.h"
#include "Logic/Expression.h"
#include <iostream>

namespace Angel::Logic 
{


void Trace::SetVerbosity(unsigned level)
{
    verbosity = level;
}

Trace::Trace(const Trace& parent, const Predicate& predicate) : 
    parent(&parent),
    verbosity(parent.verbosity)
{
    if (verbosity)
    {
        std::cout << "..." << predicate << std::endl;
    }
}


}