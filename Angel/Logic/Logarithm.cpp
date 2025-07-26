#include "Logic/Summation.h"
#include "Logic/Expression.h"

namespace Angel::Logic
{

std::ostream& operator<<(std::ostream& os, const Logarithm& operation)
{
    if (operation.size()==1)
    {
        return os << "e" <<  operation.ope << Exponentiation::OperandToString(operation.front());
    }

    bool first = true;
    for(const auto& obj: operation)
    {
        if (!first)
            os << operation.ope;
        os << Logarithm::OperandToString(obj);
        first = false;
    }
    return os;
}

}
