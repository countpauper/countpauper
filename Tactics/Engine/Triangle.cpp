#include "stdafx.h"
#include "Triangle.h"
#include "Plane.h"
#include "Line.h"
#include "Vector.h"

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

Vector Triangle::BaryCentric(const Coordinate& p) const
{
    Vector v0 = b - a;
    Vector v1 = c - a;
    Vector vp = p - a;
    auto d00 = v0.Dot(v0);
    auto d01 = v0.Dot(v1);
    auto d11 = v1.Dot(v1);
    double denom = d00 * d11 - d01 * d01;
    if (abs(denom) < std::numeric_limits<double>::epsilon())
        return Vector();    // not a triangle

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
    Plane plane(a, b, c);
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
            return sign * (c - p).Length();
        }
        else if (barycentric.z < 0)
        {
            return sign * (b - p).Length();
        }
        else
        {
            return sign * Line(b, c).Distance(p);
        }
    }
    else if (barycentric.y < 0)
    {
        if (barycentric.z < 0)
        {
            return sign * (a - p).Length();
        }
        else
        {
            return sign * Line(a, c).Distance(p);
        }
    }
    else if (barycentric.z < 0)
    {
        return sign * Line(a, b).Distance(p);
    }
    else
    {   // All>0, all should be <=1.0 too
        assert(barycentric.x <= 1.0 && barycentric.y <= 1.0 && barycentric.z <= 1.0);
        return sign * (pp - p).Length();
    }

}


std::wostream& operator<<(std::wostream& s, const Triangle& triangle)
{
    s << triangle.a << L"-" << triangle.b << L"-" << triangle.c;
    return s;
}

}