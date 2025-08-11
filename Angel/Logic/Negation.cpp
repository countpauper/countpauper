#include "Logic/Negation.h"
#include "Logic/Expression.h"

namespace Angel::Logic
{
    
class Negate : public NewUnaryOperator
{

public:
    Negate() : NewUnaryOperator(L'¬')
    {
        precedence = 40; 
        description = "negate";
        inversion = this;
    }
    Expression operator()(const Expression& operand) const override 
    {
        if (auto maybeBool = operand.TryCast<Boolean>()) 
        {
            return Boolean(!bool(*maybeBool));
        }
        else 
        {
            return Negation(operand);
        }
    }
};


GenericOperation Negation(Expression &&operand)
{
    static const Negate neg;
    return GenericOperation(neg, std::move(operand));
}

GenericOperation Negation(const Expression &operand)
{
    static const Negate neg;
    return GenericOperation(neg, {operand});
}

}
