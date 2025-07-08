#pragma once
#include "Logic/Internal/OperationBase.h"
#include "Logic/Hypothesis.h"
#include "Logic/Binary.h"
#include "Logic/Boolean.h"

namespace Angel::Logic
{

// A Disjunction is a logical operator, which is true, if any of its elements is True
class Disjunction : public OperationBase<Disjunction> 
{
public:
    Disjunction() = default;
    using OperationBase<Disjunction>::OperationBase;
    Expression Infer(const class Knowledge& k, const Hypothesis& hypothesis, Trace& trace) const;
    bool operator==(const Disjunction& rhs) const { return BaseType::operator==(rhs);}

    static constexpr BinaryOperator ope{L'∨'};
    constexpr static Boolean initial=False; 
    constexpr static Boolean final=True;
};
}