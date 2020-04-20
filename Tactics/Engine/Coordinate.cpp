#include "stdafx.h"
#include "Coordinate.h"
#include <ostream>

namespace Engine
{
    Vector operator-(const Coordinate& a, const Coordinate& b)
    {
        return Vector(a.x - b.x, a.y - b.y, a.z - b.z);
    }

    Coordinate& Coordinate::operator+=(const Vector& v)
    {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }

    std::ostream& operator<<(std::ostream& s, const Coordinate& coordinate)
    {
        s << "(" << coordinate.x << ", " << coordinate.y << ", " << coordinate.z << ")";
        return s;
    }

}


