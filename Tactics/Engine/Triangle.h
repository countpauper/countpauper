#include "Coordinate.h"
#pragma once

namespace Engine
{
    struct  Triangle
    {
    public:
        Coordinate a;
        Coordinate b;
        Coordinate c;
        Triangle() = default;
        Triangle(const Coordinate& a, const Coordinate& b, const Coordinate& c) :
            a(a),
            b(b),
            c(c)
        {
        }
        double Surface() const;
        Vector Normal() const;
        Triangle Flipped() const;
        double Distance(const Coordinate& c) const;
        Vector BaryCentric(const Coordinate& c) const;
    };

    std::ostream& operator<<(std::ostream& s, const Triangle& triangle);
}
#pragma once
