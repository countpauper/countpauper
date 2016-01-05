 #pragma once

namespace Engine
{
    struct Coordinate
    {
        Coordinate() = default;
        Coordinate(float x, float y, float z) :
            x(x),
            y(y),
            z(z)
        {
        }
        float x;
        float y;
        float z;
    };
} // ::Engine
