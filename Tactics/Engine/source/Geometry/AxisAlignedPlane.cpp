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
    return Vector(axis.GetVector()).Dot(Vector(c)) - d;
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
    if (axis.IsNone())
        return Range<double>::empty();
    auto v = Vector(axis.GetVector());
    Line translated = Matrix::Translation(v * -d) * line;

    auto dot = v.Dot(Vector(translated));
    return Range<double>(dot - d, dot - d);
}



}
