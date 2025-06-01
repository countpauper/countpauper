#include "Logic/Summation.h"
#include "Logic/Expression.h"

namespace Angel::Logic
{


const Integer Subtraction::initial(0);

std::ostream& operator<<(std::ostream& os, const Subtraction& subtraction)
{
    bool first = true;
    for(const auto& obj: subtraction)
    {
        auto s = to_string(obj);
        if (first)
            os << s;
        else if ((!s.empty()) && (s.front()=='-'))
            os << "+" << s.substr(1);
        else 
            os << subtraction.ope  << s;
        first = false;
    }
    return os;
}

}
