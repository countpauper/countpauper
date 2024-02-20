#include "Engine/Geometry/Volume.h"
#include "Engine/Geometry/Coordinate.h"
#include "Engine/Geometry/Matrix.h"
#include "Engine/Geometry/Angles.h"
#include "Engine/Geometry/AxisAlignedBoundingBox.h"
#include <numeric>
#include <cassert>

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

double Point::Intersection(const Line& line) const
{
    assert(false); // TODO
    // Probably just check if the point is colinear with the line
    // between a and b and then the distance from a
    // but it needs a unit test
    return std::numeric_limits<double>::signaling_NaN();
}

AABB Sphere::GetBoundingBox() const
{
    Vector v(radius, radius, radius);
    return AABB(center - v, v * 2.0);
}

double Sphere::Distance(const Coordinate& p) const
{
    double d = (center - p).Length() - radius;
    return d;
}

double Sphere::Volume() const
{
    return radius * radius * radius * PI * 4.0 / 3.0;
}

double Sphere::Intersection(const Line& line) const
{
    assert(false); // TODO
    return std::numeric_limits<double>::signaling_NaN();
}

Cylinder::Cylinder() :
    scale(Vector::zero),
    origin(Coordinate::origin),
    orientation(Quaternion::Identity())
{
}

Cylinder::Cylinder(const Line& axis, double dy, double dz) :
    scale(axis.Length(), dy, dz),  // unit cylinder unit length and unit radius in both directions
    origin(axis.a),
    orientation(Quaternion::Shortest(Vector(1, 0, 0), Vector(axis)))
{
}

AABB Cylinder::GetBoundingBox() const
{
    AABB box(Coordinate(0, -1, -1), Vector(1, 2, 2));   // TODO transform
    box *= Matrix::Scale(scale);
    box *= orientation.AsMatrix();
    box *= Matrix::Translation(origin);

    return box;
}

double Cylinder::Distance(const Coordinate& p) const
{
    // inverse transform to x-axis aligned cylinder : a=origin (0,0,0)
    Coordinate tp = p - origin;
    tp *= orientation.Conjugate().AsMatrix();
    // scale is not inversed, because it can't handle 0 sizes and the result will need to be transformed back in a weird way. Instead the unit cylinder is scaled
    double dy = scale.y, dz = scale.z;
    auto coefficient = tp.x / scale.x;
    auto projection = Vector(tp.x,0,0);  // p
    Vector projectionVector(0,-tp.y, -tp.z);  // pv
    // NB: this is not exactly correct, the nearest point on an elipse is not on the line between point and the center
    //  but there is no analytical solution so this is faster
    double cylinderRadius;
    if ((tp.y == 0) && (tp.z == 0))
    {
        cylinderRadius = std::min(scale.y, scale.z);
    }
    else
    {
        double a = atan2(tp.z, tp.y);
        cylinderRadius = Lerp(dz, dy, std::abs(cos(a)));
    }
    auto axisDistance = projectionVector.Length(); // |pv|

    // B          E         D
    //    |-------+----|
    // A  |   E'  | pv | } cylinderRadius
    //c<0 a-------p----b   c>1
    //    |            |   C
    //    |------------|

    if (coefficient <= 0)
    {
        auto planeDistance = -tp.x;
        if (axisDistance < cylinderRadius)
        {   // A: in tube, distance to front start plane
            return planeDistance;
        }
        else
        {   // B: outside of tube, distance to start circle
            auto tubeDistance = axisDistance - cylinderRadius;
            return sqrt(planeDistance*planeDistance + tubeDistance * tubeDistance);
        }
    }
    else if (coefficient >= 1)
    {
        auto planeDistance = tp.x - scale.x;
        if (axisDistance < cylinderRadius)
        {   // C: in tube, distance to front start plane
            return planeDistance;
        }
        else
        {   // D: outside of tube, distance to end circle
            auto tubeDistance = axisDistance - cylinderRadius;
            return sqrt(planeDistance*planeDistance + tubeDistance * tubeDistance);
        }
    }
    else
    {   // projection inside the cylinder. point either outside (E) or inside the cylinder (E')
        auto planeDistance = std::min(tp.x, scale.x - tp.x);
        return std::min(axisDistance - cylinderRadius, planeDistance);
    }
    return 0;
}

Line Cylinder::Axis() const
{
    Vector v = orientation * Vector(scale.x, 0,0);
    return Line(Coordinate::origin + origin, Coordinate::origin + origin + v);
}

Cylinder Cylinder::Slice(const Engine::Range<double>& range) const
{
    return Cylinder(Axis().Section(range), scale.y, scale.z);
}

double Cylinder::Volume() const
{
    return Engine::PI * scale.y * scale.z * scale.z;
}

double Cylinder::Intersection(const Line& line) const
{
    assert(false); // TODO
    return std::numeric_limits<double>::signaling_NaN();
}

CompoundVolume::CompoundVolume(const std::vector<std::reference_wrapper<const IVolume>>& vols)
{
    for (const auto& v : vols)
    {
        volumes.emplace_back(Engine::clone_ptr<IVolume>(v));
    }
}

CompoundVolume::CompoundVolume(const CompoundVolume& other)
{
    for (const auto& v : other.volumes)
    {
        volumes.push_back(v);
    }
}

double CompoundVolume::Intersection(const Line& line) const
{
    assert(false); // TODO
    // Probably just take the minimum of all intersections
    return std::numeric_limits<double>::signaling_NaN();
}


AABB Intersection::GetBoundingBox() const
{
    AABB box = AABB::infinity;
    for (const auto& av : volumes)
    {
        box &= av->GetBoundingBox();
    }
    return box;
}

double Intersection::Distance(const Coordinate& p) const
{
    return std::accumulate(volumes.begin(), volumes.end(), -std::numeric_limits<double>::infinity(), [&p](double value, const decltype(volumes)::value_type& av)
    {
        return std::max(value, av->Distance(p));
    });
}

double Intersection::Volume() const
{
    // TODO "Intersection volume unimplemented.");
    return std::numeric_limits<double>::quiet_NaN();
}

}
