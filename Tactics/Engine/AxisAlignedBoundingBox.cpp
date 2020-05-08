#include "stdafx.h"
#include "AxisAlignedBoundingBox.h"
#include "Vector.h"
#include "Range.h"

namespace Engine
{
AABB::AABB(const Coordinate& p0, const Coordinate& p1) :
    p0(p0),
    p1(p1)
{
}

AABB::AABB(const Coordinate& p0, const Vector& extent) :
    AABB(p0, p0 + extent)
{
}

AABB::AABB(const Range<double>& x, const Range<double>& y, const Range<double>& z) :
    p0(x.begin, y.begin, z.begin),
    p1(x.end, y.end, z.end)
{
}

Vector AABB::Extent() const
{
    return Vector(p1.x - p0.x, p1.y - p0.y, p1.z - p0.z);
}

AABB::operator bool() const
{
    return Volume()!=0;
}

double AABB::Volume() const
{
    Vector extent = Extent();
    return extent.x * extent.y * extent.z;
}

Range<double> AABB::X() const
{
    return Range(p0.x, p1.x);
}

Range<double> AABB::Y() const
{
    return  Range(p0.y, p1.y);
}

Range<double> AABB::Z() const
{
    return  Range(p0.z, p1.z);
}



AABB operator|(const AABB& a, const AABB& b)
{
    return AABB(
        a.X() | b.X(),
        a.Y() | b.Y(),
        a.Z() | b.Z());
}

AABB operator&(const AABB& a, const AABB& b)
{
    return AABB(
        a.X() & b.X(),
        a.Y() & b.Y(),
        a.Z() & b.Z());
}

}
