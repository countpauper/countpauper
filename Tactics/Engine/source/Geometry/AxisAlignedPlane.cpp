#include "Geometry/AxisAlignedPlane.h"
#include "Geometry/AxisAlignedBoundingBox.h"
#include "Geometry/Matrix.h"
namespace Engine
{

AxisAlignedPlane::operator bool() const
{
    return static_cast<bool>(axis);
}

bool AxisAlignedPlane::operator==(const AxisAlignedPlane& other) const
{
    return axis == other.axis && d == other.d;
}

AxisAlignedPlane AxisAlignedPlane::operator-() const
{
    return AxisAlignedPlane(axis.Opposite(), d);
}

AABB AxisAlignedPlane::GetBoundingBox() const
{
    if (axis.IsNone())
        return AABB();

    auto v = axis.GetVector();
    return AABB(
        v.x ? Range<double>(d,d) : Range<double>::infinity(),
        v.y ? Range<double>(d,d) : Range<double>::infinity(),
        v.z ? Range<double>(d,d) : Range<double>::infinity()
    );
}

double AxisAlignedPlane::Distance(const Coordinate& c) const
{
    double delta;
    if (axis.IsX())
        delta = c.x - d;
    else if (axis.IsY())
        delta = c.y - d;
    else if (axis.IsZ())
        delta = c.z - d;
    else
        return std::numeric_limits<double>::quiet_NaN();
    if (axis.IsNegative())
        return -delta;
    else
        return delta;
}

Coordinate AxisAlignedPlane::Project(const Coordinate& c) const
{
    if (axis.IsNone())
        throw std::runtime_error("Can't project a coordinate on a null plane.");
    auto v = axis.GetVector();
    return Coordinate(
        v.x ? d : c.x,
        v.y ? d : c.y,
        v.z ? d : c.z
    );
}

bool AxisAlignedPlane::Above(const Coordinate& c) const
{
    return Distance(c) > 0;
}

Range<double> AxisAlignedPlane::Intersection(const Line& line) const
{
    double delta, dv;
    if (axis.IsX())
    {
        delta = (d - line.o.x);
        dv = line.v.x;
    }
    else if (axis.IsY())
    {
        delta = (d - line.o.y);
        dv = line.v.y;
    }
    else if (axis.IsZ())
    {
        delta = (d - line.o.z);
        dv = line.v.z;
    }
    else
        return Range<double>::empty();

    if (dv==0)
        return Range<double>::empty();

    if (axis.IsNegative() != std::signbit(delta))
        return Range<double>(delta/dv, std::numeric_limits<double>::quiet_NaN());
    else
        return Range<double>(std::numeric_limits<double>::quiet_NaN(), delta/dv);
}



}
