#include "Logic/Trace.h"
#include "Logic/Expression.h"
#include <iostream>

namespace Angel::Logic 
{


void Trace::SetVerbosity(unsigned level)
{
    verbosity = level;
}

void Trace::operator()(const Predicate& predicate)
{
    if (verbosity)
    {
        std::cout << "..." << predicate << std::endl;
    }
}

}