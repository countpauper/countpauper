#pragma once

#include "Engine/Geometry/Quaternion.h"
#include "Engine/Geometry/Line.h"
#include "Engine/Utility/Clone.h"
#include "Engine/Geometry/Geometry.h"
#include <any>
#include <vector>

namespace Engine
{

struct AABB;

class IVolume : public IGeometry
{
public:
    bool Contains(const Coordinate& p) const { return Distance(p) <= 0; }
    virtual double Volume() const = 0;
};


class Point : public IGeometry, public Clone<Point>
{
public:
    Point(const Coordinate& c) :
        c(c)
    {}
    AABB GetBoundingBox() const override;
    double Distance(const Coordinate& p) const override;
    double Intersection(const Line& line) const override;
private:
    Coordinate c;
};

class Sphere : public IVolume, public Clone<Sphere>
{
public:
    Sphere(const Coordinate& center, double radius) :
        center(center),
        radius(radius)
    {
    }
    AABB  GetBoundingBox() const override;
    double Distance(const Coordinate& p) const override;
    double Volume() const override;
    double Intersection(const Line& line) const override;
private:
    Coordinate center;
    double radius;
};

class Cylinder : public IVolume, public Clone<Cylinder>
{
public:
    Cylinder();
    Cylinder(const Line& axis, double dy, double dz);
    AABB GetBoundingBox() const override;
    virtual double Distance(const Coordinate& p) const override;
    Line Axis() const;
    Cylinder Slice(const Engine::Range<double>& range) const;
    double Volume() const override;
    double Intersection(const Line& line) const override;
private:
    Vector scale;
    Vector origin;
    Quaternion orientation;
};


class CompoundVolume : public IVolume
{
public:
    CompoundVolume() = default;
    CompoundVolume(const std::vector<std::reference_wrapper<const IVolume>>& vols);
    CompoundVolume(const CompoundVolume& o);
    double Intersection(const Line& line) const override;
protected:
    std::vector<Engine::clone_ptr<IVolume>> volumes;
};

class Intersection : public CompoundVolume, public Clone<Intersection>
{
public:
    Intersection() = default;
    Intersection(const std::vector<std::reference_wrapper<const IVolume>>& vols) : CompoundVolume(vols) {}
    AABB GetBoundingBox() const override;
    virtual double Distance(const Coordinate& p) const override;
    double Volume() const override;
};

/*
class Union : public CompoundVolume
{

};
*/

}
