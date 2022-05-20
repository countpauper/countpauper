#pragma once
#include "Expression.h"
#include <assert.h>

namespace Angel
{
namespace Logic
{

// A colllection is a sub category of exprs consisting of multiple sub exprs
// for instance Sequence, Seqeucence, Set are Collections
class Collection : public Expression
{
public:
    Object Compute(const Knowledge& known) const
    {
        // Just return a copy. Or compute all members? 
        return Copy();
    }
};

}
}
