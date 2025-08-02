#include "Logic/Summation.h"
#include "Logic/Expression.h"
#include <cmath>

namespace Angel::Logic
{


Expression Logarithm::Simplify() const
{
    if (size()==1)
    {
        auto simple = front().Simplify();
        if (auto value = simple.TryCast<Real>())
        {
            return Real(std::log(double(*value)));
        }
    }
    return OperationBase<Logarithm>::Simplify();
}

std::ostream& operator<<(std::ostream& os, const Logarithm& operation)
{
    if (operation.size()==1)
    {
        return os << "e" <<  operation.ope << Exponentiation::OperandToString(operation.front(), false);
    }

    bool first = true;
    for(const auto& obj: operation)
    {
        if (!first)
            os << operation.ope;
        os << Logarithm::OperandToString(obj, first);
        first = false;
    }
    return os;
}

}
