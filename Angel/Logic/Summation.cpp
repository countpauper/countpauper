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
        SetCommutativeInvertible(L'-');
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
    
    void AddOperand(std::string& str, const Expression& operand) const
    {
        auto pos = str.size();
        NewBinaryOperator::AddOperand(str, operand);
        if (str.size() > pos + 2 && str[pos+1]=='-')
            str.erase(str.begin() + pos);
    }
};


GenericOperation Summation(std::initializer_list<Expression> operands)
{
    static const Sum sum;
    return GenericOperation(sum, operands);
}

}
