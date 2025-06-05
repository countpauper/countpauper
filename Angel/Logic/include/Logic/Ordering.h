#pragma once
#include "Logic/Operator.h"
#include "Logic/Internal/Comparison.h"
#include <iostream>

namespace Angel::Logic
{

template<BinaryOperator O>
class Ordering : public Comparison<Ordering<O>> 
{
public:
    using Comparison<Ordering<O>>::Comparison;
    bool operator==(const Ordering& rhs) const { return ope==rhs.ope && Comparison<Ordering<O>>::operator==(rhs); }
    constexpr static BinaryOperator ope = O;
};

using Lesser = Ordering<BinaryOperator{L'<'}>;
using LesserEqual = Ordering<BinaryOperator{L'≤'}>;
using Greater = Ordering<BinaryOperator{L'>'}>;
using GreaterEqual = Ordering<BinaryOperator{L'≥'}>;


}