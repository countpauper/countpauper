#pragma once
#include "Logic/Comparator.h"
#include "Logic/Internal/Comparison.h"
#include <iostream>

namespace Angel::Logic
{

class Equation : public Comparison<Equation> 
{
public:
    using Comparison<Equation>::Comparison;
    bool operator==(const Equation& rhs) const { return Comparison<Equation>::operator==(rhs); }
    constexpr static Comparator ope {L'='};
};

}