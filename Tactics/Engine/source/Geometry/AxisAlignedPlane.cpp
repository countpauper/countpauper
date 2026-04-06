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
        return AABB::empty;

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
        delta = c.X() - d;
    else if (axis.IsY())
        delta = c.Y() - d;
    else if (axis.IsZ())
        delta = c.Z() - d;
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
        v.x ? d : c.X(),
        v.y ? d : c.Y(),
        v.z ? d : c.Z()
    );
}

bool AxisAlignedPlane::Above(const Coordinate& c) const
{
    return Distance(c) > 0;
}

Range<double> AxisAlignedPlane::Intersection(const Line& line) const
{
    static constexpr double NaN = std::numeric_limits<double>::quiet_NaN();
    double delta, dv;
    if (axis.IsX())
    {
        delta = (d - line.o.X());
        dv = line.v.X();
    }
    else if (axis.IsY())
    {
        delta = (d - line.o.Y());
        dv = line.v.Y();
    }
    else if (axis.IsZ())
    {
        delta = (d - line.o.Z());
        dv = line.v.Z();
    }
    else
        return Range<double>(NaN,NaN);
    if (dv == 0)
        return Range<double>(std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity());

    if (axis.IsNegative() != std::signbit(dv))
        return Range<double>(delta/dv, NaN);
    else
        return Range<double>(NaN, delta/dv);
}



}
