#include "Coordinate.h"
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
    Coordinate Project(const Coordinate& p) const;
    AABB Bounds() const;
};

std::wostream& operator<<(std::wostream& s, const Line& line);

}
