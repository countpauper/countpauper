#include "stdafx.h"
#include "Volume.h"
#include "Coordinate.h"
#include "Matrix.h"
#include <numeric>

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
    return AABB(center - v, v * 2.0);
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
        return -std::min(std::min(d.x, d.y), d.z);
    }
    else
    {
        Vector d(std::max(0.0, std::max(x.begin - p.x, p.x - x.end)),
            std::max(0.0, std::max(y.begin - p.y, p.y - y.end)),
            std::max(0.0, std::max(z.begin - p.z, p.z - z.end)));
        return d.Length();
    }
}


AABB Cylinder::GetBoundingBox() const
{
    AABB box(Coordinate(0, -dy, -dz), Vector(xaxis.Length(), 2 * dy, 2 * dz));

    Vector vx = Vector(xaxis).Normal();
    Vector vy = Vector(vx.y, vx.x, 0).Normal(); // perpendicular in the xy plane
    Vector vz = vx.Cross(vy);
    Matrix trans(vx, vy, vz, Vector(xaxis.a));
    box *= trans;
    return box;
}

double Cylinder::Distance(const Coordinate& p) const
{
    auto coefficient = xaxis.ProjectionCoefficient(p);  // c
    auto projection = xaxis.a + Vector(xaxis) * coefficient;  // p
    auto projectionVector = projection - p;  // pv
    auto cylinderRadius = sqrt(dy*dz);   // TODO: at the right angle 
    auto axisDistance = projectionVector.Length(); // |pv|


    // B          E         D
    //    |-------+----|
    // A  |   E'  | pv | } cylinderRadius
    //c<0 a-------p----b   c>1 
    //    |            |   C
    //    |------------|

    if (coefficient <= 0)
    {
        auto planeDistance = -coefficient * Vector(xaxis).Length();
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
        auto planeDistance = 1.0 - coefficient * Vector(xaxis).Length();
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
        auto planeDistance = std::min(coefficient, 1.0-coefficient) * xaxis.Length();
        return std::min(axisDistance - cylinderRadius, planeDistance);
    }
}



AABB Intersection::GetBoundingBox() const
{
    AABB box = AABB::infinity;
    for (const auto& av : volumes)
    {
        if (const IVolume* v = std::any_cast<IVolume>(&av))
        {
            box &= v->GetBoundingBox();
        }
    }
    return box;
}

double Intersection::Distance(const Coordinate& p) const 
{
    return std::accumulate(volumes.begin(), volumes.end(), std::numeric_limits<double>::infinity(), [&p](double value, const std::any& av)
    {
        if (const IVolume* v = std::any_cast<IVolume>(&av))
        {
            return std::max(value, v->Distance(p));
        }
        else
        {
            return value;
        }
        
    });
}


}