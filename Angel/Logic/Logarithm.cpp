#include "Logic/Logarithm.h"
#include "Logic/Expression.h"
#include <cmath>
#include <bit>

namespace Angel::Logic
{

class NaturalLog : public NewUnaryOperator
{
public:
    NaturalLog() : NewUnaryOperator(L'↓')
    {
        precedence = 71; 
        description = "natural logarithm";
        SetInvertible(L'↑');
    }

    Expression operator()(const Expression& operand) const override 
    {
        if (auto maybeNumber = operand.GetIf<Number>()) 
        {
            if (auto maybeReal = maybeNumber->GetIf<Real>())
            {
                return Real(std::log(double(*maybeReal)));
            }
            else if (auto maybeInt = maybeNumber->GetIf<Integer>())
            {   // power of 2 ?
                long v = long(*maybeInt);
                if (v>0)
                    return Integer(std::bit_width((unsigned long)(v)) - 1);
                assert(false);  // negative power 2 not supported (yet);
            }
            else if (auto maybeBool = maybeNumber->GetIf<Boolean>())
            {
                if (bool(*maybeBool))
                    return Real(0.0);
                else 
                    return Real(-std::numeric_limits<double>::infinity());
            }
            else 
            {
                assert(false);  // complex (or whatever) not yet implemented
            }
        }
        else 
        {
            return Logarithm({operand});
        }
    } 

    void AddOperand(std::string& str, const Expression& operand) const
    {
        str += "e" + std::string(*this);
        if (NeedsBracesAround(operand, str.empty()))
            str += std::format("({})", to_string(operand));
        else 
            str += to_string(operand);
    }    
};

class Log : public NewBinaryOperator
{
public:
    Log() : NewBinaryOperator(L'↓', true)
    {
        static NaturalLog _ln;
        unary = &_ln;
        precedence = 70; 
        description = "logarithm";
        commutative = false;
        SetInvertible(L'↑');
    }
    Expression operator()(const Expression& lhs, const Expression& rhs) const override 
    {
        if (const auto* lhNumber = lhs.GetIf<Number>())
        {
            return log(*lhNumber, rhs.Cast<Number>());
        }
        else 
        {
            assert(false); // possibly missing infer or shouldn't get here. Or perhaps during simplify? 
            return Logarithm({lhs, rhs});
        }
    }
};


GenericOperation Logarithm(std::initializer_list<Expression> operands)
{
    static const Log log;
    return GenericOperation(log, operands);
}

}
