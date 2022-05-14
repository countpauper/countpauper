#pragma once
#include "Expression.h"
#include <assert.h>

namespace Angel
{
namespace Logic
{

// A colllection is a sub category of exprs consisting of multiple sub exprs
// for instance Array, Seqeucence, Set are Collections
class Collection : public Expression
{
public:
    Object Compute(const Knowledge& known) const override
    {
        assert(false); // TODO: copy all elements? 
        return Object();

    }
};

}
}
