#pragma once

#include "AxisAlignedBoundingBox.h"
namespace Engine
{

class IVolume
{
public:
    // bounding box around volume, no points outside this bounding box are inside the volume
    virtual AABB  GetBoundingBox() const = 0;
    // Distance from a point to the volume. distance==0 means the surface of the volume. distance<0 means inside the volume
    virtual double Distance(const Coordinate& p) const = 0;
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

}