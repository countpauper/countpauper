#include "Logic/Element.h"
#include "Logic/Knowledge.h"
#include <assert.h>

namespace Angel::Logic
{

Object Element::Copy() const
{
    // Elements can be copied by casting them to themselves with no knowledge;
    Knowledge none;
    return Cast(typeid(*this), none);
}

Match Element::Matching(const Expression& other) const
{
    if(other == *this) 
        return IsMatch;
    else
        return NoMatch;    
}

Object Element::Infer(const Knowledge& known, const Variables&) const
{
    return Copy();
}

}

