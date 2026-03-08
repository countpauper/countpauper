#pragma once
#include "Geometry/Geometry.h"
#include "Geometry/Orientation.h"
#include "Utility/Clone.h"

namespace Engine
{


struct AxisAlignedPlane : public IGeometry, public Clone<AxisAlignedPlane>
{
    AxisAlignedPlane(Orientation axis, double d=0.0) :
        axis(axis),
        d(d)
    {
    }

    explicit operator bool() const;
    bool operator==(const AxisAlignedPlane& other) const;
    AxisAlignedPlane operator-() const;

    AABB GetBoundingBox() const override;
    double Distance(const Coordinate& c) const override;
    Range<double> Intersection(const Line& line) const override;
    Coordinate Project(const Coordinate& c) const;
    bool Above(const Coordinate& c) const;
    // Line Project(const Line& l) const;
    // bool IsParallel(const Line& line) const;
    Orientation axis;
    double d;

    static const AxisAlignedPlane null;
    static const AxisAlignedPlane xy;
    static const AxisAlignedPlane xz;
    static const AxisAlignedPlane yz;
};
}
