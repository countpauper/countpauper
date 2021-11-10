#pragma once
#include "Position.h"

namespace Physics
{
    struct Size : Position
    {
        Size() = default;

        Size(int x, int y, int z) : Position(x, y, z)
        {
        }
        explicit Size(int s) : Size(s, s, s) {}
        explicit Size(const Position& p) : Position(p)
        {
        }
        int Volume() const { return x * y * z; }
    };

    Size operator+(const Size& a, const Size& b);
    Size operator+(const Size& a, int s);
}   // ::Physics

