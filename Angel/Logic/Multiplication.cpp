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
        commutative = true;
        SetInvertible(L'÷');
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
    std::string OperandToString(const Expression& operand, bool first) const
    {
        std::string result = NewBinaryOperator::OperandToString(operand, first);
        if (result.size()>2 && result[1]=='-')
            result = result.substr(1);
        return result;
    }
};


GenericOperation Multiplication(std::initializer_list<Expression> operands)
{
    static const Multiply mul;
    return GenericOperation(mul, operands);
}

}
