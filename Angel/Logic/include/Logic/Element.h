#pragma once
#include "Logic/Boolean.h"
#include "Logic/Integer.h"
#include "Logic/Id.h"


namespace Angel::Logic
{

using Element = std::variant<Boolean, Integer, Id>
}
