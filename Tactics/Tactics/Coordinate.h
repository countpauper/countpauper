 #pragma once

namespace Engine
{
    struct Coordinate
    {
        Coordinate() = default;
        Coordinate(float x, float y, float z) :
            x(x),
            y(y),
            z(z)
        {
        }
        float x;
        float y;
        float z;
    };

    struct Vector
    {
        Vector() = default;
        Vector(float x, float y, float z) :
            x(x),
            y(y),
            z(z)
        {
        }
        Vector(const Vector& v) :
            x(v.x),
            y(v.y),
            z(v.z)
        {
        }

        Vector(const Coordinate& c) :
            x(c.x),
            y(c.y),
            z(c.z)
        {
        }

        Vector& operator=(const Vector& o)
        {
            x = o.x;
            y = o.y;
            z = o.z;
            return *this;
        }
        operator bool() const
        {
            return x != 0 || y != 0 || z != 0;
        }
        double Length() const
        {
            return sqrt(SqrLength());
        }
        double SqrLength() const
        {
            return x*x + y*y + z*z;
        }
        float x;
        float y;
        float z;
    };

    Vector operator-(const Coordinate& a, const Coordinate& b);
} // ::Engine
