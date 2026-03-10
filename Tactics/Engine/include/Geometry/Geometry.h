#pragma once
#include "Utility/Range.h"

namespace Engine
{

class Line;
class Coordinate;
struct AABB;

class IGeometry
{
public:
    // bounding box around volume, no points outside this bounding box are inside the volume
    virtual AABB GetBoundingBox() const = 0;
    // Distance from a point to the volume. distance==0 means the surface of the volume. distance<0 means inside the volume
    virtual double Distance(const Coordinate& p) const = 0;
    // first is the distance from A where the line first enters the volume and end from where it exits or the length to B
    // This distance is in multiples of the Line vector's vector so for a linesegment [0...1]
    // For a half line with a normalized vector it's distance from the line's origin.
    // For ininite lines, the range can contain negative values.
    // When entering a plane with no volume, but with a normal, like Plane or Triangle, the begin is used when entering the plane
    //  in the direction of the normal and the end is used when exiting from behind. The other end is NaN. Use Range::GetNumber()
    // returns empty if there is no intersection at all
    virtual Range<double> Intersection(const Line& line) const = 0;
};

}
