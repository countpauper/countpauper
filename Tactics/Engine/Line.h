#include "Coordinate.h"
#include "Range.h"
#pragma once

namespace Engine
{

struct AABB;

struct  Line
{
public:
    Coordinate a;
    Coordinate b;
    Line() = default;
    Line(const Coordinate& a, const Coordinate& b) :
        a(a),
        b(b)
    {
    }
    double LengthSquared() const;
    double Length() const;
    double Distance(const Coordinate& p) const;
    double ProjectionCoefficient(const Coordinate& p) const;
    Line Section(const Engine::Range<double>& range) const;
    Coordinate Project(const Coordinate& p) const;
    AABB Bounds() const;
};

std::ostream& operator<<(std::ostream& s, const Line& line);

}
