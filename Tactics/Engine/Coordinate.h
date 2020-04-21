 #pragma once

namespace Engine
{

    struct Vector;

    struct Coordinate
    {
        Coordinate() = default;
        Coordinate(float x, float y, float z) :
            x(x),
            y(y),
            z(z)
        {
        }
        Coordinate& operator+=(const Vector& v);
        Coordinate& operator-=(const Vector& v);
        Coordinate& operator*=(float factor);

        float x;
        float y;
        float z;
    };
    Coordinate operator*(const Coordinate& c, float factor);
    Coordinate operator*(const Coordinate& c, double factor);
    Coordinate operator+(const Coordinate& c, const Vector& v);
    Coordinate operator-(const Coordinate& c, const Vector& v);

    std::ostream& operator<<(std::ostream& s, const Coordinate& coordinate);
    std::wostream& operator<<(std::wostream& s, const Coordinate& coordinate);
    std::wistream& operator>>(std::wistream& s, Coordinate& coordinate);

    struct Vector
    {
        Vector() = default;
        Vector(float x, float y, float z) :
            x(x),
            y(y),
            z(z)
        {
        }
        Vector(const Vector& v) = default;

        Vector(const Coordinate& c) :
            x(c.x),
            y(c.y),
            z(c.z)
        {
        }

        Vector& operator=(const Vector& o);
        operator bool() const;
        double Length() const;
        double LengthSquared() const;
        bool operator==(const Vector& o) const;
        Vector& operator*=(float factor);
        Vector& operator+=(const Vector& v);
        Vector& operator-=(const Vector& v);

        double Dot(const Vector& v) const;

        float x;
        float y;
        float z;
    };
    Vector operator*(const Vector& v, float factor);
    Vector operator*(const Vector& v, double factor);
    Vector operator+(const Vector& a, const Vector& b);
    Vector operator-(const Vector& a, const Vector& b);
    Vector operator-(const Coordinate& a, const Coordinate& b);

} // ::Engine
