#pragma once

#include "Logic/Expression.h"
#include "Logic/Binary.h"
#include <array>
#include <cassert>

namespace Angel::Logic
{

template<BinaryOperator OP> 
constexpr Expression operate(const Expression&, const Expression&);

}
