#include "stdafx.h"
#include "Expression.h"
#include "Object.h"

namespace Angel
{
namespace Logic
{



bool Expression::Match(const Expression& other, const Knowledge&) const
{
	return operator==(other);
}


Object Expression::Compute(const Knowledge& known) const
{
    return Object(); 
}

}
}