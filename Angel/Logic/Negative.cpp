#include "Logic/Negative.h"
#include "Logic/Expression.h"

namespace Angel::Logic
{
    
class NegativeOp : public NewUnaryOperator
{

public:
    NegativeOp() : NewUnaryOperator(L'-')
    {
        precedence = 75; 
        description = "negative";
        inversion = this;
    }
    Expression operator()(const Expression& operand) const override 
    {
        if (auto maybeNumber = operand.GetIf<Number>()) 
        {
            return -(*maybeNumber);
        }
        else 
        {
            return Negative(operand);
        }
    }
};

GenericOperation Negative(Expression &&operand)
{
    static const NegativeOp neg;
    return GenericOperation(neg, std::move(operand));
}

GenericOperation Negative(const Expression &operand)
{
    static const NegativeOp neg;
    return GenericOperation(neg, {operand});
}

}
