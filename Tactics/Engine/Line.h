#include "Coordinate.h"
#pragma once

namespace Engine
{
class Line
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
    Engine::Vector Vector() const;
};

double Distance(const Coordinate& p, const Line& l);

}
