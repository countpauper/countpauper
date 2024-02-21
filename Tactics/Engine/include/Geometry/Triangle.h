#include "Geometry/Coordinate.h"
#pragma once

namespace Engine
{
    class Line;

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
        // NaN if line does not intersect the triangle
        // positive distance from line.a to the interseciont if line.a is above the plane of the counter-clockwise triangle
        // negative distance from line.a to the intersection if line.a is below the plane of the counter-clockwise triangle
        double Intersection(const Line& line) const;
        Vector BaryCentric(const Coordinate& c) const;
    };

    std::ostream& operator<<(std::ostream& s, const Triangle& triangle);
}
#pragma once
