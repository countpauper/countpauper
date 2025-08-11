#include "Logic/Summation.h"
#include "Logic/Expression.h"

namespace Angel::Logic
{

class Sum : public NewBinaryOperator
{

public:
    Sum() : NewBinaryOperator(L'+', true)
    {
        static const Expression _identity{Integer(0)};
        identity = &_identity;
        precedence = 50; 
        description = "add";
    }
    Expression operator()(const Expression& lhs, const Expression& rhs) const override 
    {
        if (const auto* lhNumber = lhs.GetIf<Number>())
        {
            return *lhNumber + rhs.Cast<Number>();
        }
        else if (const auto* lhContainer = lhs.GetIf<Container>())
        {
            return *lhContainer + rhs.Cast<Container>();
        }
        else 
        {
            assert(false); // possibly missing infer or shouldn't get here. Or perhaps during simplify? 
            return Summation({lhs, rhs});
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


GenericOperation Summation(std::initializer_list<Expression> operands)
{
    static const Sum sum;
    return GenericOperation(sum, operands);
}

}
