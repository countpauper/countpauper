#pragma once
#include "Vector.h"
#include "Volume.h"

namespace Engine
{

struct Coordinate;
struct Line;

struct Plane : public IVolume
{
    // a*normal.x + b*normal.y + c*normal.z + d = 0;
    Plane(const Engine::Vector& n, double d) :
        normal(n),
        d(d)
    {
    }
    Plane(const Coordinate& origin, const Vector& n);
    Plane(const Coordinate& origin, const Vector& span1, const Vector& span2);
    Plane(const Coordinate& a, const Coordinate& b, const Coordinate& c);

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
    double Volume() const override { return std::numeric_limits<double>::infinity(); }
    bool IsParallel(const Line& line) const;
    Coordinate Intersection(const Line& line) const;
    Engine::Vector normal;  // does not need to be normalized 
    double d;

    static Plane null;
    static Plane xy;
    static Plane xz;
    static Plane yz;
private:
    double NormalDistance(const Coordinate& c) const;
};
}