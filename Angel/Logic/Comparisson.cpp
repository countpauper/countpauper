#include "Logic/Internal/IntComparison.h"

namespace Angel::Logic
{

bool IsRedundantComparisson(const Expression& left, const Expression& right)
{
    if (left.Assumptions())
        return false;
    if (right.Assumptions())
        return false;
    return true;
}


}