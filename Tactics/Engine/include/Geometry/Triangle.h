#include "Geometry/Coordinate.h"
#include "Geometry/Geometry.h"

#pragma once

namespace Engine
{
    class Line;

    struct  Triangle : public IGeometry
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
        Vector BaryCentric(const Coordinate& c) const;

        // IGeometry
        AABB GetBoundingBox() const override;
        double Distance(const Coordinate& p) const;
        Range<double> Intersection(const Line& line) const override;

    };

    std::ostream& operator<<(std::ostream& s, const Triangle& triangle);
}
#pragma once
