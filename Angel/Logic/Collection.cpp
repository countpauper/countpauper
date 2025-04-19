#include "Logic/Collection.h"

namespace Angel::Logic
{

Object Collection::Compute([[maybe_unused]] const Knowledge& known) const
{
    // Just return a copy. Or compute all members? 
    return Copy();
}

}
