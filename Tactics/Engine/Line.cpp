#include "stdafx.h"
#include "Line.h"
#include "Maths.h"
#include "Vector.h"
#include "AxisAlignedBoundingBox.h"

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

double Line::ProjectionCoefficient(const Coordinate& p) const
{
    Engine::Vector v = p - a;
    double length_squared = LengthSquared();
    if (length_squared == 0)
        return 0;   // project on point a==b 
    return Vector(*this).Dot(v) / length_squared;
}

Coordinate Line::Project(const Coordinate& p) const
{
    double interpolation_factor = ProjectionCoefficient(p);
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
    Coordinate projection = Project(p);
    return (p - projection).Length();
}


AABB Line::Bounds() const
{
    return AABB(a, b);
}

std::wostream& operator<<(std::wostream& s, const Line& line)
{
    s << line.a << L"-" << line.b;
    return s;
}

}