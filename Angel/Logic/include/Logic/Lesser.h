#pragma once
#include "Logic/Comparator.h"
#include "Logic/Internal/Comparison.h"

namespace Angel::Logic
{

class Lesser : public Comparison<Lesser>
{
public:
    using Comparison<Lesser>::Comparison;
    constexpr static Comparator ope{L'<'};
};

}
