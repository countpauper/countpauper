#pragma once 

#include "Logic/Expression.h"

namespace Angel::Logic
{
// TODO: for now object is just the same as expression.
// Later Object should be fully computed expressions. So this can be 
//  Elements and Containers of Objects
// It can not be: predicates, variables, operations (eg conjunctions)
// Infer would return Objects
// Containers should have two versions: expression containers that contain other expression 
// and object containers

class Object : public Expression
{
public:
    using Expression::Expression;
};

}