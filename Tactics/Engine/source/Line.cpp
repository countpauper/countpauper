#include "Engine/Line.h"
#include "Engine/Maths.h"
#include "Engine/Vector.h"
#include "Engine/AxisAlignedBoundingBox.h"
#include <sstream>

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


Line Line::Section(const Engine::Range<double>& range) const
{
    Range<double> slice = range / Length();
    slice &= Range(0.0, 1.0);
    auto v = Vector(*this);
    return Line(a + v * slice.begin, a + v * slice.end);
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
        return Coordinate::origin+Lerp(Vector(a), Vector(b), interpolation_factor);
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

bool Line::operator==(const Line& rhs) const
{
    return a == rhs.a && b == rhs.b;
}


std::ostream& operator<<(std::ostream& s, const Line& line)
{
    s << line.a << "-" << line.b;
    return s;
}

}