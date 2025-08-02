#pragma once
#include "Logic/Comparator.h"
#include "Logic/Internal/Comparison.h"

namespace Angel::Logic
{

class In : public Comparison<In>
{
public:
    using Comparison<In>::Comparison;
    constexpr static Comparator ope{L'∈'};
};

}
