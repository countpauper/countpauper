#include "stdafx.h"
#include "Vector.h"
#include "Coordinate.h"
#include "Line.h"
#include "Plane.h"
#include "Matrix.h"

namespace Engine
{


Vector::Vector(const Coordinate& c) :
    x(c.x),
    y(c.y),
    z(c.z)
{
}

Vector::Vector(const Line& l) : 
    Vector(l.b - l.a)
{
}

Vector::Vector(const Plane& p) :
    Vector(p.normal)
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

Vector& Vector::operator*=(const Matrix& m)
{
    *this = m * *this;
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

std::ostream& operator<<(std::ostream& s, const Vector& v)
{
    Coordinate c(v.x, v.y, v.z);
    return s << c;
}

std::wostream& operator<<(std::wostream& s, const Vector& v)
{
    Coordinate c(v.x, v.y, v.z);
    return s << c;
}

std::wistream& operator>>(std::wistream& s, Vector& v)
{
    Coordinate c;
    s >> c;
    v = Vector(c.x, c.y, c.z);
    return s;
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

Vector operator*(const Matrix& m, const Vector& v)
{
    double w = v.x * m[0][3] + v.y * m[1][3] + v.z * m[2][3] + m[3][3];
    return Vector(
        v.x * m[0][0] + v.y * m[1][0] + v.z * m[2][0] + m[3][0],
        v.x * m[0][1] + v.y * m[1][1] + v.z * m[2][1] + m[3][1],
        v.x * m[0][2] + v.y * m[1][2] + v.z * m[2][2] + m[3][2]
    ) / w;
}

}


