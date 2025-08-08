#pragma once
#include "Logic/Internal/GenericOperation.h"
#include <iostream>

namespace Angel::Logic
{

GenericOperation Summation(std::initializer_list<Expression> operands);

}