#pragma once
#include "Logic/Comparator.h"
#include "Logic/Internal/Comparison.h"

namespace Angel::Logic
{

class Equal : public Comparison<Equal>
{
public:
    using Comparison<Equal>::Comparison;
    constexpr static Order ope{L'='};
};

}
