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
    // NaN if parallel or no intersection.
    // positive distance if A is above the plane or outside the volume
    // negative distance if A is below the plane or outside the volume
    virtual double Intersection(const Line& line) const = 0;
};

}
