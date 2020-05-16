#include "stdafx.h"
#include "Line.h"
#include "Maths.h"
#include "Vector.h"

namespace Engine
{

double Line::LengthSquared() const
{
    return Vector(*this).LengthSquared();
}

double Line::Length() const
{
    return Vector(*this).Length();
}

Coordinate Line::Nearest(const Coordinate& p) const
{
    Engine::Vector v = p - a;
    double length_squared = LengthSquared();
    if (length_squared == 0)
        return a;

    double interpolation_factor = Vector(*this).Dot(v) / length_squared;
    if (interpolation_factor <= 0)
    {
        return a;
    }
    else if (interpolation_factor >= 1)
    {
        return b;
    }
    else
    {
        return Coordinate::zero+Lerp(Vector(a), Vector(b), interpolation_factor);
    }
}
double Line::Distance(const Coordinate& p) const
{
    Coordinate nearest = Nearest(p);
    return (p - nearest).Length();
}


}