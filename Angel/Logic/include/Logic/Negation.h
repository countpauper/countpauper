#pragma once
#include "Logic/Internal/GenericOperation.h"
#include <iostream>

namespace Angel::Logic
{

GenericOperation Negation(Expression&& operand);
GenericOperation Negation(const Expression& operand);

}