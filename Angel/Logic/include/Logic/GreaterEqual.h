#pragma once
#include "Logic/Comparator.h"
#include "Logic/Internal/Comparison.h"

namespace Angel::Logic
{

class GreaterEqual : public Comparison<GreaterEqual>
{
public:
    using Comparison<GreaterEqual>::Comparison;
    constexpr static Comparator ope{L'≥'};
};

}
