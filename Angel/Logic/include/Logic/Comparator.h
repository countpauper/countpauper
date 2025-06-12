#pragma once 
#include "Logic/Binary.h"

namespace Angel::Logic
{

class Comparator : public BinaryOperator
{
public:
    using BinaryOperator::BinaryOperator;
    constexpr bool operator()(const Expression& lhs, const Expression& rhs) const;
};

}