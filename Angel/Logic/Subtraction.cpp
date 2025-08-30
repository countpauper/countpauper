#include "Logic/Summation.h"
#include "Logic/Expression.h"

namespace Angel::Logic
{

class Subtract : public NewBinaryOperator
{

public:
    Subtract() : NewBinaryOperator(L'-', true)
    {
        static const Expression _identity{Integer(0)};
        identity = &_identity;
        precedence = 50; 
        description = "subtract";
        commutative = false;
        SetInvertible(L'+');
    }
    Expression operator()(const Expression& lhs, const Expression& rhs) const override 
    {
        if (const auto* lhNumber = lhs.GetIf<Number>())
        {
            return *lhNumber - rhs.Cast<Number>();
        }
        else  if (const auto* lhContainer = lhs.GetIf<Container>())
        {
            return *lhContainer - rhs.Cast<Container>();
        }
        else 
        {
            assert(false); // possibly missing infer or shouldn't get here. Or perhaps during simplify? 
            return Subtraction({lhs, rhs});
        }
    }
    std::string OperandToString(const Expression& operand, bool first) const
    {
        std::string result = NewBinaryOperator::OperandToString(operand, first);
        if (result.size()>2 && result[1]=='-')
            result = std::string("+") + result.substr(2);
        return result;
    }    
};


GenericOperation Subtraction(std::initializer_list<Expression> operands)
{
    static const Subtract subtract;
    return GenericOperation(subtract, operands);
}

}
