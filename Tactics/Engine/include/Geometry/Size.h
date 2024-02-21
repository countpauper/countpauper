#pragma once
#include "Geometry/Position.h"

namespace Engine
{
    struct Size : Position
    {
        Size() = default;

        Size(int x, int y, int z) : Position(x, y, z)
        {
        }
        explicit Size(int s) : Size(s, s, s) {}
        explicit Size(Position p) : Position(p)
        {
        }
        int Volume() const { return x * y * z; }
    };

    Size operator+(Size a, Size b);
    Size operator+(Size a, int s);
}   // ::Engine

