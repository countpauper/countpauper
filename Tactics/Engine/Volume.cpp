#include "stdafx.h"
#include "Volume.h"

namespace Engine
{
AABB Point::GetBoundingBox() const
{
    return AABB(c, c);
}

double Point::Distance(const Coordinate& p) const
{
    return (c - p).Length();
}

AABB Sphere::GetBoundingBox() const
{
    Vector v(radius, radius, radius);
    return AABB(center- v, v * 2.0);
}

double Sphere::Distance(const Coordinate& p) const
{
    double d = (center - p).Length() - radius;
    return d;
}

}