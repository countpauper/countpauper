#include "Logic/Multiplication.h"
#include "Logic/Expression.h"

namespace Angel::Logic
{

class Multiply : public NewBinaryOperator
{
public:
    Multiply() : NewBinaryOperator(L'⋅', true)
    {
        static const Expression _identity{Integer(1)};
        identity = &_identity;
        static const Expression _absorb{Integer(0)};
        absorb = &_absorb;
        precedence = 60; 
        description = "multiply";
        SetCommutativeInvertible(L'÷');
    }
    Expression operator()(const Expression& lhs, const Expression& rhs) const override 
    {
        if (const auto* lhNumber = lhs.GetIf<Number>())
        {
            return *lhNumber * rhs.Cast<Number>();
        }
        else 
        {
            assert(false); // possibly missing infer or shouldn't get here. Or perhaps during simplify? 
            return Multiplication({lhs, rhs});
        }
    }
};


GenericOperation Multiplication(std::initializer_list<Expression> operands)
{
    static const Multiply mul;
    return GenericOperation(mul, operands);
}

}
