#pragma once
#include "Logic/GenericOperation.h"
#include <iostream>

namespace Angel::Logic
{

GenericOperation Negative(Expression&& operand);
GenericOperation Negative(const Expression& operand);

}