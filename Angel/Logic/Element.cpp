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


Object Element::Match(const Expression& other) const
{
    return boolean((*this) == other);
}

}

