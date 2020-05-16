#include "stdafx.h"
#include "Plane.h"
#include "Coordinate.h"
#include "Line.h"
#include "Maths.h"

namespace Engine
{

    Plane::Plane(const Coordinate& origin, const Engine::Vector& n) :
        normal(n),
        d(-Vector(origin).Dot(n))
    {
    }
    Plane::Plane(const Coordinate& origin, const Engine::Vector& span1, const Engine::Vector& span2) : 
        Plane(origin, span1 * span2)    // cross product
    {
    }

    Plane::Plane(const Coordinate& a, const Coordinate& b, const Coordinate& c) :
        Plane(a, b-a, c-a)
    {
    }

    Coordinate Plane::Project(const Coordinate& c) const
    {
        return c - normal * NormalDistance(c) / normal.LengthSquared();
    }

    double Plane::NormalDistance(const Coordinate& c) const
    {
        return normal.x * c.x + normal.y * c.y + normal.z * c.z + d;
    }

    bool Plane::Above(const Coordinate& c) const
    {
        return NormalDistance(c) > 0;
    }

    double Plane::Distance(const Coordinate& c) const
    {
        return std::abs(NormalDistance(c) / normal.Length());
    }

    bool Plane::IsParallel(const Line& line) const
    {
        double dot = Vector(line)*normal;
        return dot == 0;
    }

    Coordinate Plane::Intersection(const Line& line) const
    {
        double dot = Vector(line)*normal;
        if (dot < std::numeric_limits<double>::epsilon())
            throw std::runtime_error("Line is parallel to plane");
        Vector lineVector(line);
        double aDot = (lineVector * normal);
        double interpolationFactor = aDot / dot;
        return line.a + lineVector * interpolationFactor;
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
        auto dot = normal * other.normal;
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


Plane Plane::null(Vector(0, 0, 0), 0);
Plane Plane::xy(Vector(0, 0, 1), 0);
Plane Plane::xz(Vector(0, 1, 0), 0);
Plane Plane::yz(Vector(1, 0, 0), 0);


}
