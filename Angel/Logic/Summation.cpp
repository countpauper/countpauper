#include "Logic/Summation.h"
#include "Logic/Expression.h"

namespace Angel::Logic
{

std::ostream& operator<<(std::ostream& os, const Summation& operation)
{
    bool first = true;
    for(const auto& obj: operation)
    {
        auto s = to_string(obj);
        if ((!first) && (!s.empty()) && (s.front()!='-'))
            os << operation.ope;
        os << s;
        first = false;
    }
    return os;
}

}
