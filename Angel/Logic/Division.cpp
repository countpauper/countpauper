#include "Logic/Division.h"
#include "Logic/Expression.h"

namespace Angel::Logic
{

class Divide : public NewBinaryOperator
{
public:
    Divide() : NewBinaryOperator(L'÷', true)
    {
        static const Expression _identity{Integer(1)};
        identity = &_identity;
        // TODO 0 is left absorbing 
        precedence = 60; 
        description = "divide";
        SetInvertible(L'⋅', L'÷');
    }
    Expression operator()(const Expression& lhs, const Expression& rhs) const override 
    {
        if (const auto* lhNumber = lhs.GetIf<Number>())
        {
            return *lhNumber / rhs.Cast<Number>();
        }
        else 
        {
            assert(false); // possibly missing infer or shouldn't get here. Or perhaps during simplify? 
            return Division({lhs, rhs});
        }
    }
};


GenericOperation Division(std::initializer_list<Expression> operands)
{
    static const Divide div;
    return GenericOperation(div, operands);
}

}
