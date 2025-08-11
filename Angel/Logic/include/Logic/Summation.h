#pragma once
#include "Logic/GenericOperation.h"
#include <iostream>

namespace Angel::Logic
{

GenericOperation Summation(std::initializer_list<Expression> operands);

}