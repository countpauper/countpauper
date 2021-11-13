#pragma once

#include "AxisAlignedBoundingBox.h"
#include "Line.h"

namespace Engine
{

class IVolume
{
public:
    // bounding box around volume, no points outside this bounding box are inside the volume
    virtual AABB GetBoundingBox() const = 0;
    // Distance from a point to the volume. distance==0 means the surface of the volume. distance<0 means inside the volume
    virtual double Distance(const Coordinate& p) const = 0;
    // Check whether the volume contains a point
    bool Contains(const Coordinate& p) const { return Distance(p) <= 0; }
};


class Point : public IVolume
{
public:
    Point(const Coordinate& c) : 
        c(c) 
    {}
    AABB  GetBoundingBox() const override;
    double Distance(const Coordinate& p) const override;
private:
    Coordinate c;
};

class Sphere : public IVolume
{
public:
    Sphere(const Coordinate& center, double radius) :
        center(center),
        radius(radius)
    {    
    }
    AABB  GetBoundingBox() const override;
    double Distance(const Coordinate& p) const override;
private:
    Coordinate center;
    double radius;
};

class AABox : public AABB, public IVolume
{
public:
    AABox(const Coordinate& a, const Coordinate& b) : AABB(a, b) {}
    AABox(const Coordinate& c, const Vector& e) : AABB(c, e) {}
    AABox(const Range<double>& x, const Range<double>& y, const Range<double>& z) : AABB(x, y, z) {}

    // IVolume;
    AABB GetBoundingBox() const override { return *this; }
    virtual double Distance(const Coordinate& p) const override;
};

class Cylinder : public IVolume
{
public:
    Cylinder(const Line& axis, double dx, double dy) : axis(axis), dx(dx), dy(dy) {}
    AABB GetBoundingBox() const override;   // TODO. nb check tilted ends /----/
    virtual double Distance(const Coordinate& p) const override; // TODO: distance to projection on line. subtract diameter vector, nb tilted in axis direction
private:
    Line axis;
    double dx;
    double dy;

};
}