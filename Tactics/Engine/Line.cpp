#include "stdafx.h"
#include "Line.h"
#include "Geometry.h"

namespace Engine
{

double Line::LengthSquared() const
{
    return Vector().LengthSquared();
}

double Line::Length() const
{
    return Vector().Length();
}

Engine::Vector Line::Vector() const
{
    return b - a;
}

double Distance(const Coordinate& p, const Line& l)
{
    Vector v = p - l.a;
    double length_squared = l.LengthSquared();
    if (length_squared == 0)
        return v.Length();

    double interpolation_factor = l.Vector().Dot(v) / length_squared;

    if (interpolation_factor <= 0)
    {
        return v.Length();
    }
    else if (interpolation_factor >= 1)
    {
        return (l.b - p).Length();
    }
    else
    {
        Engine::Vector nearest = Lerp(Vector(l.a), Vector(l.b), interpolation_factor);
        return (Vector(p)-nearest).Length();
    }
}


}