#include "stdafx.h"
#include "Coordinate.h"

namespace Engine
{
    Vector operator-(const Coordinate& a, const Coordinate& b)
    {
        return Vector(a.x - b.x, a.y - b.y, a.z - b.z);
    }
}

