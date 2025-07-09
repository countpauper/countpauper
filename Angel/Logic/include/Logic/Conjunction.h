#pragma once
#include "Logic/Internal/OperationBase.h"
#include "Logic/Hypothesis.h"
#include "Logic/Multiary.h"
#include "Logic/Boolean.h"

namespace Angel::Logic
{

// A Conjunction is a logical operator, which is true, only if all its elements are True
// It is currently a Collection (ordered, non-unique) because the ordering is used to determine lazy evaulation
// but technically it could be ordered and unique. 
class Conjunction : public OperationBase<Conjunction> 
{
public:
    Conjunction() = default;
    using OperationBase<Conjunction>::OperationBase;
    Expression Infer(const class Knowledge& k, const Hypothesis& hypothesis, Trace& trace) const;
    bool operator==(const Conjunction& rhs) const { return BaseType::operator==(rhs);}

    constexpr static MultiOperator ope {L'∧'};
    constexpr static Boolean initial=True; 
    constexpr static Boolean final=False;
};


}