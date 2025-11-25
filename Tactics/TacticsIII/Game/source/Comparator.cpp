#include "Game/Comparator.h"
#include <cassert>

namespace Game
{


Comparator::Comparator(Id id)
    : id(id)
{
}

Comparator Comparator::Opposite() const
{
    switch(id)
    {
    case not_equal:
        return Comparator(equal);
    case equal:
        return Comparator(not_equal);
    case less:
        return Comparator(greater_equal);
    case less_equal:
        return Comparator(greater);
    case greater:
        return Comparator(less_equal);
    case greater_equal:
        return Comparator(less);
    default:
        assert(false); // unimplemented
        return *this;
    }
}

bool Comparator::operator()(int actual, int required) const
{
    switch(id)
    {
        case not_equal:
            return actual != required;
        case equal:
            return actual == required;
        case less:
            return actual < required;
        case less_equal:
            return actual <= required;
        case greater:
            return actual > required;
        case greater_equal:
            return actual >= required;
        default:
            assert(false); // unimplemented
            return false;
    }
}

bool Comparator::operator==(Comparator rhs) const
{
    return id == rhs.id;
}

Comparator::operator std::string() const
{
    switch(id)
    {
        case not_equal:
            return "is not";
        case equal:
            return "is";
        case less:
            return "is less than";
        case less_equal:
            return "is not more than";
        case greater:
            return "is more than";
        case greater_equal:
            return "is not less than";
        default:
            assert(false); // unimplemented
            return "";
    }
}

}
