#include "stdafx.h"
#include "Volume.h"
#include "Coordinate.h"

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

double AABox::Distance(const Coordinate& p) const
{
    if (GetBoundingBox().Contains(p))
    {
        Vector d(std::min(p.x - x.begin, x.end - p.x),
                 std::min(p.y - y.begin, y.end - p.y),
                 std::min(p.z - z.begin, z.end - p.z));
        return -std::min(std::min(d.x, d.y), d.z );
    }
    else
    {
        Vector d(std::max(0.0, std::max(x.begin - p.x, p.x - x.end)),
                 std::max(0.0, std::max(y.begin - p.y, p.y - y.end)),
                 std::max(0.0, std::max(z.begin - p.z, p.z - z.end)));
        return d.Length();
    }
}

}