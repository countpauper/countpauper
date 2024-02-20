#include "Engine/Geometry/Plane.h"
#include "Engine/Geometry/Coordinate.h"
#include "Engine/Geometry/Line.h"
#include "Engine/Utility/Maths.h"
#include "Engine/Geometry/Triangle.h"
#include "Engine/Geometry/AxisAlignedBoundingBox.h"

namespace Engine
{

    Plane::Plane(const Coordinate& origin, const Engine::Vector& n) :
        normal(n),
        d(-Vector(origin).Dot(n))
    {
    }
    Plane::Plane(const Coordinate& origin, const Engine::Vector& span1, const Engine::Vector& span2) :
        Plane(origin, span1.Cross(span2))    // cross product
    {
    }

    Plane::Plane(const Coordinate& a, const Coordinate& b, const Coordinate& c) :
        Plane(a, b-a, c-a)  // ccw is front face
    {
    }

    Plane::Plane(const Triangle& t) :
        Plane(t.a, t.b, t.c)
    {
    }

    Coordinate Plane::Project(const Coordinate& c) const
    {
        return c - normal * NormalDistance(c) / normal.LengthSquared();
    }

    Line Plane::Project(const Line& l) const
    {
        return Line(Project(l.a), Project(l.b));
    }

    double Plane::NormalDistance(const Coordinate& c) const
    {
        return normal.x * c.x + normal.y * c.y + normal.z * c.z + d;
    }

    bool Plane::Above(const Coordinate& c) const
    {
        return NormalDistance(c) > 0;
    }

    AABB Plane::GetBoundingBox() const
    {
        // if the normal is perpendicular to an axis, then it is limited
        auto result = AABB::infinity;
        if (normal.y == 0 && normal.z == 0)
        {   // perendicular to x
            if (normal.x < 0)
            {
                result.x.begin = -d / normal.x;
            }
            else
            {
                result.x.end = -d / normal.x;
            }
        }
        else if (normal.x == 0 && normal.z == 0)
        {   // perpendicular to y
            if (normal.y < 0)
            {
                result.y.begin = -d / normal.y;
            }
            else
            {
                result.y.end = -d / normal.y;
            }
        }
        else if (normal.x == 0 && normal.y == 0)
        {   // perpendicular to z
            if (normal.z < 0)
            {
                result.z.begin = -d / normal.z;
            }
            else
            {
                result.z.end = -d / normal.z;
            }
        }
        return result;
    }


    double Plane::Distance(const Coordinate& c) const
    {
        return NormalDistance(c) / normal.Length();
    }

    bool Plane::IsParallel(const Line& line) const
    {
        double dot = Vector(line).Cross(normal);
        return dot == 0;
    }

    double Plane::Intersection(const Line& line) const
    {
        // TODO: this can probably be optimized with a
        // method similar to Triangle, just don't check the limtis on u and v

        double normaldot = Vector(line).Dot(normal);
        if (std::abs(normaldot) < std::numeric_limits<double>::epsilon())
            return std::numeric_limits<double>::quiet_NaN();
        // P is projection vector from line.a to a point on the plane
        Vector P =  normal * -NormalDistance(line.a) / normal.LengthSquared();
        double projectiondot = P.Dot(normal);
        double fraction = projectiondot / normaldot;
        if ((fraction<0) || (fraction>1))
            return std::numeric_limits<double>::quiet_NaN();
        if (normaldot < 0 )
            return fraction * line.Length();    // a is above the plane
        else
            return -fraction * line.Length();   // a is behind the plane
    }

    Plane& Plane::Normalize()
    {
        double length = normal.Length();
        if (length < std::numeric_limits<double>::epsilon())
            return *this;
        d /= length;
        normal /= length;
        return *this;
    }

    Plane Plane::Normalized() const
    {
        return Plane(*this).Normalize();
    }

    Plane::operator bool() const
    {
        return normal;
    }

    bool Plane::operator==(const Plane& other) const
    {
        auto dot = normal.Cross(other.normal);
        if (dot != 0)
            return false;
        if (d == 0 && other.d == 0)
            return true;
        return (normal / d) == (other.normal / other.d);
    }

    Plane Plane::operator-() const
    {
        return Plane(-normal, -d);
    }


Plane Plane::null(Vector::zero, 0);
Plane Plane::xy(Vector(0, 0, 1), 0);
Plane Plane::xz(Vector(0, 1, 0), 0);
Plane Plane::yz(Vector(1, 0, 0), 0);


}
