
#include "Engine/Coordinate.h"
#include "Engine/Line.h"
#include "Engine/Plane.h"
#include "Engine/Matrix.h"
#include "Engine/Quaternion.h"

namespace Engine
{


Vector::Vector(Coordinate c) :
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
        return Vector::zero;
    return Vector(x / l, y / l, z / l);
}

Vector Vector::Cross(Vector o) const
{
    return Vector(
        y*o.z - z * o.y,
        z*o.x - x * o.z,
        x*o.y - y * o.x
    );
}


bool Vector::operator==(Vector o) const
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

Vector& Vector::operator+=(Vector v)
{
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
}

Vector& Vector::operator-=(Vector v)
{
    x -= v.x;
    y -= v.y;
    z -= v.z;
    return *this;
}

Vector& Vector::operator*=(const Matrix& m)
{
    *this = m * *this;
    return *this;
}

Vector& Vector::operator*=(const Quaternion& q)
{
    *this = q* *this;
    return *this;
}

Vector Vector::operator-() const
{
    return Vector(-x, -y, -z);
}

double Vector::Dot(Vector v) const
{
    return x * v.x + y * v.y + z * v.z;
}

const Vector Vector::zero(0, 0, 0);

std::ostream& operator<<(std::ostream& s, Vector v)
{
    Coordinate c(v.x, v.y, v.z);
    return s << c;
}

std::istream& operator>>(std::istream& s, Vector& v)
{
    Coordinate c;
    s >> c;
    v = Vector(c.x, c.y, c.z);
    return s;
}


Vector operator*(Vector v, double factor)
{
    return Vector(v) *= factor;
}

Vector operator/(Vector v, double factor)
{
    return Vector(v) /= factor;
}

Vector operator+(Vector a, Vector b)
{
    return Vector(a) += b;
}

Vector operator-(Vector a, Vector b)
{
    return Vector(a) -= b;
}

Vector operator-(Coordinate a, Coordinate b)
{
    return Vector(a.x - b.x, a.y - b.y, a.z - b.z);
}

Vector operator*(const Matrix& m, Vector v)
{
    double w = v.x * m[0][3] + v.y * m[1][3] + v.z * m[2][3] + m[3][3];
    return Vector(
        v.x * m[0][0] + v.y * m[1][0] + v.z * m[2][0] + m[3][0],
        v.x * m[0][1] + v.y * m[1][1] + v.z * m[2][1] + m[3][1],
        v.x * m[0][2] + v.y * m[1][2] + v.z * m[2][2] + m[3][2]
    ) / w;
}


}


