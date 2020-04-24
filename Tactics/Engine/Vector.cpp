#include "stdafx.h"
#include "Vector.h"
#include "Coordinate.h"

namespace Engine
{


Vector::Vector(const Coordinate& c) :
    x(c.x),
    y(c.y),
    z(c.z)
{
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

Vector Vector::Normal() const
{
    auto l = Length();
    if (l == 0)
        return Vector(0, 0, 0);
    return Vector(x / l, y / l, z / l);
}

bool Vector::operator==(const Vector& o) const
{
    return x == o.x && y == o.y && z == o.z;
}

Vector& Vector::operator*=(double factor)
{
    x *= factor;
    y *= factor;
    z *= factor;
    return *this;
}

Vector& Vector::operator/=(double divisor)
{
    x /= divisor;
    y /= divisor;
    z /= divisor;
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

Vector& Vector::operator*=(const Vector& o)
{
    *this = *this * o;
    return *this;
}

Vector Vector::operator-() const
{
    return Vector(-x, -y, -z);
}



double Vector::Dot(const Vector& v) const
{
    return x * v.x + y * v.y + z * v.z;
}

Vector operator*(const Vector& v, double factor)
{
    return Vector(v) *= factor;
}

Vector operator/(const Vector& v, double factor)
{
    return Vector(v) /= factor;
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
Vector operator*(const Vector& a, const Vector& b)
{
    return Vector(
        a.y * b.z - a.z*b.y,
        a.z * b.x - a.x*b.z,
        a.x * b.y - a.y*b.x
    );
}
}


