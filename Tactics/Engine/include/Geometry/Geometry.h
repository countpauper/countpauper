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
    // Empty range if parallel or no intersection.
    // first is the distance from A where the line first enters the volume and end from where it exits or the length to B
    // returns empty if there is no intersection at all()
    virtual Range<double> Intersection(const Line& line) const = 0;
};

}
