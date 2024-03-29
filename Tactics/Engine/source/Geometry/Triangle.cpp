#include "Geometry/Triangle.h"
#include "Geometry/Plane.h"
#include "Geometry/Line.h"
#include "Geometry/Vector.h"
#include <cassert>


namespace Engine
{


double Triangle::Surface() const
{
    Vector ab = b - a;
    Vector ac = c - a;
    double d = ab.Dot(ac);
    if (d == 0) // perpendicular at a
        return ab.Length()*ac.Length()*0.5;
    d/= ab.LengthSquared();
    Coordinate p = a+ab * d;
    return 0.5 * (p - c).Length() * (b - a).Length();
}

Vector Triangle::Normal() const
{
    Plane plane(a, b, c);
    return plane.normal;
}

Triangle Triangle::Flipped() const
{
    return Triangle(a, c, b);
}


Vector Triangle::BaryCentric(const Coordinate& p) const
{   // https://gamedev.stackexchange.com/questions/23743/whats-the-most-efficient-way-to-find-barycentric-coordinates
    Vector v0 = b - a;
    Vector v1 = c - a;
    Vector vp = p - a;
    auto d00 = v0.Dot(v0);
    auto d01 = v0.Dot(v1);
    auto d11 = v1.Dot(v1);
    double denom = d00 * d11 - d01 * d01;
    if (abs(denom) < std::numeric_limits<double>::epsilon())
        throw std::runtime_error("Barycentric coordinates of degenerate triangle");

    auto dp0 = vp.Dot(v0);
    auto dp1 = vp.Dot(v1);
    Vector barycentric;
    barycentric.y = (d11*dp0 - d01 * dp1) / denom;
    barycentric.z = (d00*dp1 - d01 * dp0) / denom;
    barycentric.x = 1.0 - barycentric.y - barycentric.z;
    return barycentric;
}

double Triangle::Distance(const Coordinate& p) const
{
    Plane plane(a, b, c);   // counter clockwise
    if (!plane)
    {
        return sqrt(std::min((a - p).LengthSquared(), std::min((b - p).LengthSquared(), (c - p).LengthSquared())));
    }
    Coordinate pp = plane.Project(p);
    Vector barycentric = BaryCentric(pp);
    double sign = std::signbit(plane.normal.Dot(p - pp)) ? -1 : 1;

    if (barycentric.x < 0)
    {
        if (barycentric.y < 0)
        {
            return (c - p).Length();
        }
        else if (barycentric.z < 0)
        {
            return (b - p).Length();
        }
        else
        {
            return Line(b, c).Distance(p);
        }
    }
    else if (barycentric.y < 0)
    {
        if (barycentric.z < 0)
        {
            return (a - p).Length();
        }
        else
        {
            return Line(a, c).Distance(p);
        }
    }
    else if (barycentric.z < 0)
    {
        return Line(a, b).Distance(p);
    }
    else
    {   // All>0, all should be <=1.0 too
        assert(barycentric.x <= 1.0 && barycentric.y <= 1.0 && barycentric.z <= 1.0);
        // If it projects into the triangle a negative value is returned if the point is behind the triangle
        // this allows at least convex meshes to determine if the point is inside the mesh when all distance<=0
        return sign * (pp - p).Length();
    }

}


double Triangle::Intersection(const Line& line) const
{
    // See https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
    constexpr double epsilon = std::numeric_limits<double>::epsilon();
    constexpr double nan = std::numeric_limits<double>::quiet_NaN();
    Vector ray_vector = Vector(line).Normal();
    Vector edge1 = b - a;
    Vector edge2 = c - a;
    Vector ray_cross_e2 = ray_vector.Cross(edge2);
    double det = edge1.Dot(ray_cross_e2);

    if (det > -epsilon && det < epsilon)
        return nan;    // This ray is parallel to this triangle.

    double inv_det = 1.0 / det;
    Vector s = line.a - a;
    double u = inv_det * s.Dot(ray_cross_e2);

    if (u < 0 || u > 1)
        return nan;     // the ray passes through the triangle's plane outside of the bounds

    Vector s_cross_e1 = s.Cross(edge1);
    double v = inv_det * ray_vector.Dot(s_cross_e1);

    if (v < 0 || u + v > 1)
        return nan;  // the ray passes through the triangle's plane outside of the boudnds


    // At this stage we can compute t to find out where the intersection point is on the line.
    double t = inv_det * edge2.Dot(s_cross_e1);

    if (t >= 0) // ray intersection
        if (det > 0)
            return t;
        else
            return -t;
    else // This means that there is a line intersection but not a ray intersection.
        return nan;
}


std::ostream& operator<<(std::ostream& s, const Triangle& triangle)
{
    s << triangle.a << "-" << triangle.b << "-" << triangle.c;
    return s;
}

}
