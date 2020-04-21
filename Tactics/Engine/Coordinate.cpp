#include "stdafx.h"
#include "Coordinate.h"
#include <ostream>

namespace Engine
{


Coordinate& Coordinate::operator+=(const Vector& v)
{
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
}

Coordinate& Coordinate::operator-=(const Vector& v)
{
    x -= v.x;
    y -= v.y;
    z -= v.z;
    return *this;
}

Coordinate& Coordinate::operator*=(float factor)
{
    x *= factor;
    y *= factor;
    z *= factor;
    return *this;
}

Coordinate operator*(const Coordinate& c, float factor)
{
    return Coordinate(c) *= factor;
}
Coordinate operator*(const Coordinate& c, double factor)
{
    return c * float(factor);
}
Coordinate operator+(const Coordinate& c, const Vector& v)
{
    return Coordinate(c) += v;
}
Coordinate operator-(const Coordinate& c, const Vector& v)
{
    return Coordinate(c) -= v;
}

std::ostream& operator<<(std::ostream& s, const Coordinate& coordinate)
{
    s << "(" << coordinate.x << ", " << coordinate.y << ", " << coordinate.z << ")";
    return s;
}




Vector& Vector::operator=(const Vector& o)
{
    x = o.x;
    y = o.y;
    z = o.z;
    return *this;
}
Vector::operator bool() const
{
    return x != 0 || y != 0 || z != 0;
}
double Vector::Length() const
{
    return sqrt(LengthSquared());
}
double Vector::LengthSquared() const
{
    return x * x + y * y + z * z;
}

bool Vector::operator==(const Vector& o) const
{
    return x == o.x && y == o.y && z == o.z;
}

Vector& Vector::operator*=(float factor)
{
    x *= factor;
    y *= factor;
    z *= factor;
    return *this;
}

Vector& Vector::operator+=(const Vector& v)
{
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
}

Vector& Vector::operator-=(const Vector& v)
{
    x -= v.x;
    y -= v.y;
    z -= v.z;
    return *this;
}

double Vector::Dot(const Vector& v) const
{
    return double(x) * v.x + double(y) * v.y + double(z) * v.z; 
}

Vector operator*(const Vector& v, float factor)
{
    return Vector(v) *= factor;
}

Vector operator*(const Vector& v, double factor)
{
    return v * float(factor);
}

Vector operator+(const Vector& a, const Vector& b)
{
    return Vector(a) += b;
}

Vector operator-(const Vector& a, const Vector& b)
{
    return Vector(a) -= b;
}

Vector operator-(const Coordinate& a, const Coordinate& b)
{
    return Vector(a.x - b.x, a.y - b.y, a.z - b.z);
}
}


