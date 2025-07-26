#include "Logic/Summation.h"
#include "Logic/Expression.h"

namespace Angel::Logic
{

std::ostream& operator<<(std::ostream& os, const Exponentiation& operation)
{
    if (operation.size()==1)
    {
        return os << "e" <<  operation.ope << Exponentiation::OperandToString(operation.front());
    }
    
    if (operation.size()==2)
    {
        if (operation.back()==Integer(2))
        {
            return os << Exponentiation::OperandToString(operation.front()) << "²";
        }
        else if (operation.back()==Integer(3))
        {
            return os << Exponentiation::OperandToString(operation.front()) << "³";
        }
/*
        else if (operation.back() == Real(0.5))
        {
            return os << "√" << Exponentiation::OperandToString(operation.front());
        }
*/
    }
    bool first = true;
    for(const auto& obj: operation)
    {
        if (!first)
            os << operation.ope;
        os << Exponentiation::OperandToString(obj);
        first = false;
    }
    return os;
}

}
