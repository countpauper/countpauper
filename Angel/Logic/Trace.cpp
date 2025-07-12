#include "Logic/Trace.h"
#include "Logic/Expression.h"
#include <iostream>

namespace Angel::Logic 
{


void Trace::SetVerbosity(unsigned level)
{
    verbosity = level;
    if (parent)
        parent->verbosity = verbosity;  // for siblings, else this does very little
}

Trace::Trace(Trace& parent, const Predicate& predicate) : 
    parent(&parent),
    verbosity(parent.verbosity)
{
    if (verbosity)
    {
        for(auto d=0; d < Depth();++d)
            std::cout << "…";
        std::cout << predicate << std::endl;
    }
}

unsigned Trace::Depth() const
{
    unsigned depth = 0;
    for(auto p = parent; p; p=p->parent)
        depth++;
    return depth;
}


}