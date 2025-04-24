#pragma once
#include "Expression.h"
#include <assert.h>

namespace Angel
{
namespace Logic
{

// A colllection is a sub category of exprs consisting of multiple sub exprs
// for instance Sequence, Set are Collections
class Collection : public Expression
{
public:
    Object Infer([[maybe_unused]] const Knowledge& known, const Variables& substitutions) const;
    const Object* Condition() const override;
};

}
}
