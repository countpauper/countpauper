#include "Logic/Exponentiation.h"
#include "Logic/Expression.h"
#include <cmath>

namespace Angel::Logic
{

class Exp : public NewUnaryOperator
{
public:
    Exp() : NewUnaryOperator(L'↑')
    {
        precedence = 71; 
        description = "exponent";
        SetInvertible(L'↓');
    }

    Expression operator()(const Expression& operand) const override 
    {
        if (auto maybeNumber = operand.GetIf<Number>()) 
        {
            if (auto maybeReal = maybeNumber->GetIf<Real>())
            {
                return Real(std::exp(double(*maybeReal)));
            }
            else if (auto maybeInt = maybeNumber->GetIf<Integer>())
            {   // power of 2 ?
                return Integer(1 << long(*maybeInt));
            }
            else if (auto maybeBool = maybeNumber->GetIf<Boolean>())
            {
                if (bool(*maybeBool))
                    return Real(1.0);
                else 
                    return Real(std::exp(1.0));
            }
            else 
            {
                assert(false);  // complex (or whatever) not yet implemented
            }
        }
        else 
        {
            return Exponentiation({operand});
        }
    } 

    std::string OperandToString(const Expression& operand, bool first) const
    {
        std::string result;
        if (NeedsBracesAround(operand, first))
            result = std::format("({})", to_string(operand));
        else 
            result = to_string(operand);
        return std::format("e{}{}", std::string(*this), result);
    }
};

class Power : public NewBinaryOperator
{
public:
    Power() : NewBinaryOperator(L'↑', true)
    {
        static const Expression _identity{Integer(1)};
        identity = &_identity;
        static const Exp _exp;
        unary = &_exp;
        precedence = 70; 
        description = "power";
        
        commutative = false;
        SetInvertible(L'↓');
    }
    Expression operator()(const Expression& lhs, const Expression& rhs) const override 
    {
        if (const auto* lhNumber = lhs.GetIf<Number>())
        {
            return *lhNumber ^ rhs.Cast<Number>();
        }
        else 
        {
            assert(false); // possibly missing infer or shouldn't get here. Or perhaps during simplify? 
            return Exponentiation({lhs, rhs});
        }
    }
};


GenericOperation Exponentiation(std::initializer_list<Expression> operands)
{
    static const Power pow;
    return GenericOperation(pow, operands);
}

}
