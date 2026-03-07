#pragma once
#include "Geometry/Volume.h"
#include "Geometry/"
namespace Engine
{


struct AxisAlignedPlane : public IGeometry, public Clone<Plane>
{
    Plane(const Engine::Vector& n, double d) :
        normal(n),
        d(d)
    {
    }
    Plane(const Coordinate& origin, const Vector& n);
    Plane(const Coordinate& origin, const Vector& span1, const Vector& span2);
    Plane(const Coordinate& a, const Coordinate& b, const Coordinate& c);
    Plane(const Triangle& t);

    Plane& Normalize();
    Plane Normalized() const;
    operator bool() const;
    bool operator==(const Plane& other) const;
    Plane operator-() const;

    Coordinate Project(const Coordinate& c) const;
    Line Project(const Line& l) const;
    bool Above(const Coordinate& c) const;
    AABB GetBoundingBox() const override;
    double Distance(const Coordinate& c) const override;
    bool IsParallel(const Line& line) const;
    Range<double> Intersection(const Line& line) const override;
    Engine::Vector normal;  // not necessarily be normalized
    double d;

    static Plane null;
    static Plane xy;
    static Plane xz;
    static Plane yz;
private:
    double NormalDistance(const Coordinate& c) const;
};
}
