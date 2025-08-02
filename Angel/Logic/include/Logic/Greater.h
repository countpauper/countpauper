#pragma once
#include "Logic/Comparator.h"
#include "Logic/Internal/Comparison.h"

namespace Angel::Logic
{

class Greater : public Comparison<Greater>
{
public:
    using Comparison<Greater>::Comparison;
    constexpr static Order ope{L'>'};
};

}
