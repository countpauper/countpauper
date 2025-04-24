#include "Logic/Collection.h"

namespace Angel::Logic
{

Object Collection::Infer([[maybe_unused]] const Knowledge& known, const Variables&) const
{
    // Just return a copy. Or Infer all members? 
    return Copy();
}

}
