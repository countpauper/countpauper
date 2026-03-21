#include "Geometry/Plane.h"
#include "Geometry/Coordinate.h"
#include "Geometry/Line.h"
#include "Utility/Maths.h"
#include "Geometry/Triangle.h"
#include "Geometry/AxisAlignedBoundingBox.h"
#include "Geometry/AxisAlignedPlane.h"

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

    Plane::Plane(const AxisAlignedPlane& p) :
        Plane(Vector(p.axis.GetVector()), p.d)
    {

    }


    Coordinate Plane::Project(const Coordinate& c) const
    {
        if (!bool(normal))
            throw std::runtime_error("Can't project a coordinate on a null plane.");
        return c - normal * NormalDistance(c) / normal.LengthSquared();
    }

    Line Plane::Project(const Line& l) const
    {
        return Line(Project(l.A()), Project(l.B()));
    }

    double Plane::NormalDistance(const Coordinate& c) const
    {
        return normal.X() * c.X() + normal.Y() * c.Y() + normal.Z() * c.Z() + d;
    }

    bool Plane::Above(const Coordinate& c) const
    {
        return NormalDistance(c) > 0;
    }

    AABB Plane::GetBoundingBox() const
    {
        // if the normal is perpendicular to an axis, then it is limited
        auto result = AABB::infinity;
        if (normal.Y() == 0 && normal.Z() == 0)
        {   // perendicular to x
            if (normal.X() < 0)
            {
                result.x.begin = -d / normal.X();
            }
            else
            {
                result.x.end = -d / normal.X();
            }
        }
        else if (normal.X() == 0 && normal.Z() == 0)
        {   // perpendicular to y
            if (normal.Y() < 0)
            {
                result.y.begin = -d / normal.Y();
            }
            else
            {
                result.y.end = -d / normal.Y();
            }
        }
        else if (normal.X() == 0 && normal.Y() == 0)
        {   // perpendicular to z
            if (normal.Z() < 0)
            {
                result.z.begin = -d / normal.Z();
            }
            else
            {
                result.z.end = -d / normal.Z();
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
        double dot = Vector(line).Dot(normal);
        return dot == 0;
    }

    Range<double> Plane::Intersection(const Line& line) const
    {
        // TODO: this can probably be optimized with a
        // method similar to Triangle, just don't check the limtis on u and v

        double normaldot = Vector(line).Dot(normal);
        if (std::abs(normaldot) < std::numeric_limits<double>::epsilon())
            return Range<double>::empty();
        // P is projection vector from line.a to a point on the plane
        Vector P =  normal * -NormalDistance(line.o) / normal.LengthSquared();
        double projectiondot = P.Dot(normal);
        double fraction = projectiondot / normaldot;

        if (std::signbit(projectiondot))
            return Range<double>(fraction, std::numeric_limits<double>::quiet_NaN());
        else
            return Range<double>(std::numeric_limits<double>::quiet_NaN(), fraction);
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
        return bool(normal);
    }

    bool Plane::operator==(const Plane& other) const
    {
        auto cross = normal.Cross(other.normal);
        if (bool(cross))
            return false;
        if (d == 0 && other.d == 0)
            return true;
        return (normal / d) == (other.normal / other.d);
    }

    Plane Plane::operator-() const
    {
        return Plane(-normal, -d);
    }

}
