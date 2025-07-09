#pragma once
#include "Logic/Comparator.h"
#include "Logic/Internal/Comparison.h"

namespace Angel::Logic
{

class LesserEqual : public Comparison<LesserEqual>
{
public:
    using Comparison<LesserEqual>::Comparison;
    constexpr static Comparator ope{L'≤'};
};

}
