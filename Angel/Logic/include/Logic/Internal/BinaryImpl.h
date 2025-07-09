#pragma once

#include "Logic/Expression.h"
#include "Logic/Binary.h"
#include <array>
#include <cassert>

namespace Angel::Logic
{

template<BinaryOperator OP> 
constexpr Expression operate(const Expression&, const Expression&);
 
constexpr Expression BinaryOperator::operator()(const Expression&, const Expression&) const
{
    assert(false);  // these are currently implemented direction in expressions like Associations, containers
    return Expression(); 
}

}
