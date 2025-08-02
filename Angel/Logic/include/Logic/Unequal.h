#pragma once
#include "Logic/Comparator.h"
#include "Logic/Internal/Comparison.h"

namespace Angel::Logic
{

class Unequal : public Comparison<Unequal>
{
public:
    using Comparison<Unequal>::Comparison;
    constexpr static Order ope{L'≠'};
};

}
