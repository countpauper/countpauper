#include "stdafx.h"
#include "AxisAlignedBoundingBox.h"
#include "Vector.h"
#include "Range.h"
#include "Line.h"

namespace Engine
{


const AABB AABB::infinity(Range<double>::infinity(), Range<double>::infinity(), Range<double>::infinity());
const AABB AABB::empty(Range<double>::null(), Range<double>::null(), Range<double>::null());

AABB::AABB(const Coordinate& begin, const Coordinate& end) :
    x(begin.x, end.x),
    y(begin.y, end.y),
    z(begin.z, end.z)
{
}

AABB::AABB(const Line& line) :
    AABB(line.a, line.b)
{
}

AABB::AABB(const Coordinate& begin, const Vector& extent) :
    AABB(begin, begin + extent)
{
}

AABB::AABB(const Range<double>& x, const Range<double>& y, const Range<double>& z) :
    x(x),
    y(y),
    z(z)
{
}

Coordinate AABB::Begin() const
{
    return Coordinate(x.begin, y.begin, z.begin);
}
Coordinate AABB::End() const
{
    return Coordinate(x.end, y.end, z.end);
}

Coordinate AABB::Center() const
{
    return Coordinate(x.Middle(), y.Middle(), z.Middle());

}
Vector AABB::Extent() const
{
    return Vector(x.Size(), y.Size(), z.Size());
}

bool AABB::Contains(const Coordinate& p) const
{
    return x[p.x] && y[p.y] && z[p.z];
}

Coordinate AABB::Clip(const Coordinate& p) const
{
    return Coordinate(x.Clip(p.x), y.Clip(p.y), z.Clip(p.z));
}




AABB& AABB::operator*=(const Matrix& transformation)
{
    auto begin = transformation * Begin();
    auto end = transformation * End();
    *this = AABB(begin, end);

    return *this;
}

AABB& AABB::operator+=(const Vector& offset)
{
    x += offset.x;
    y += offset.y;
    z += offset.z;
    return *this;
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
    return x;
}

Range<double> AABB::Y() const
{
    return y;
}

Range<double> AABB::Z() const
{
    return z;
}

AABB& AABB::operator|=(const Coordinate& p)
{
    x |= p.x;
    y |= p.y;
    z |= p.z;
    return *this;
}

AABB& AABB::operator&=(const AABB& o)
{
    *this = (*this) & o;
    return *this;
}

AABB& AABB::Expand(const Engine::Vector& v)
{
    x.Expand(v.x);
    y.Expand(v.y);
    z.Expand(v.z);
    return *this;
}

AABB& AABB::Grow(double v)
{
    return Grow(Vector(v, v, v));
}

AABB& AABB::Grow(const Engine::Vector& v)
{
    x.Grow(v.x);
    y.Grow(v.y);
    z.Grow(v.z);
    return *this;
}

AABB operator|(const AABB& a, const AABB& b)
{
    return AABB(
        a.X() | b.X(),
        a.Y() | b.Y(),
        a.Z() | b.Z());
}

AABB operator|(const AABB& a, const Coordinate& c)
{
    return AABB(a) |= c;
}

AABB operator&(const AABB& a, const AABB& b)
{
    return AABB(
        a.X() & b.X(),
        a.Y() & b.Y(),
        a.Z() & b.Z());
}

}
