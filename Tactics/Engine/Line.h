#include "Coordinate.h"
#pragma once

namespace Engine
{

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
    Coordinate Nearest(const Coordinate& p) const;
};

}
