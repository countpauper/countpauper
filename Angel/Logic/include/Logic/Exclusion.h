#pragma once
#include "Logic/Internal/OperationBase.h"
#include "Logic/Hypothesis.h"
#include "Logic/Multiary.h"
#include "Logic/Boolean.h"

namespace Angel::Logic
{

// An exclusion  is a logical operator, which is true, only if exactly one of its elements is True
class Exclusion : public OperationBase<Exclusion> 
{
public:
    Exclusion() = default;
    using OperationBase<Exclusion>::OperationBase;
    Expression Infer(const class Knowledge& k, const Hypothesis& hypothesis, Trace& trace) const;
    bool operator==(const Exclusion& rhs) const { return BaseType::operator==(rhs);}

    constexpr static MultiOperator ope {L'⊕'};
    constexpr static Boolean identity=False; 
};


}