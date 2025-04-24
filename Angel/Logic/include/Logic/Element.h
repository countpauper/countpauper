#pragma once
#include "Expression.h"

namespace Angel::Logic
{

// An element is a sub category of Expression that is not a compound of other Expressions
// For instance an Integer, Boolean is an Element
class Element : public Expression
{
public:
    Object Copy() const override;
    Match Matching(const Expression& other, const Variables& substitutions) const override;
    const Object* Condition() const override;

    Object Infer(const Knowledge& known, const Variables& substitutions) const;
};

}
