
#include "Geometry/Coordinate.h"
#include "Geometry/Line.h"
#include "Geometry/Plane.h"
#include "Geometry/Matrix.h"
#include "Geometry/Quaternion.h"
#include "Geometry/Orientation.h"

namespace Engine
{


Vector::Vector(Coordinate c) :
    x(c.X()),
    y(c.Y()),
    z(c.Z())
{
}

Vector::Vector(const Position& p) :
    x(p.x),
    y(p.y),
    z(p.z)
{
}

Vector::Vector(const Line& l) :
    Vector(l.v)
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

double Vector::Axis(const Orientation& axis) const
{
    if (axis.IsX())
        return X();
    if (axis.IsY())
        return Y();
    if (axis.IsZ())
        return Z();
    else
        return std::numeric_limits<double>::quiet_NaN();
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

bool Vector::IsNormalized() const
{
    return std::abs(1.0 - LengthSquared()) < 10.0 * std::numeric_limits<double>::epsilon();
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
    x += v.X();
    y += v.Y();
    z += v.Z();
    return *this;
}

Vector& Vector::operator-=(Vector v)
{
    x -= v.X();
    y -= v.Y();
    z -= v.Z();
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
    return x * v.X() + y * v.Y() + z * v.Z();
}

std::ostream& operator<<(std::ostream& s, Vector v)
{
    Coordinate c(v.X(), v.Y(), v.Z());
    return s << c;
}

std::istream& operator>>(std::istream& s, Vector& v)
{
    Coordinate c;
    s >> c;
    v = Vector(c.X(), c.Y(), c.Z());
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
    return Vector(a.X() - b.X(), a.Y() - b.Y(), a.Z() - b.Z());
}

Vector operator*(const Matrix& m, Vector v)
{
    auto [vx, vy, vz ] = v.Coefficients();
    double w = vx * m[0][3] + vy * m[1][3] + vz * m[2][3] + m[3][3];
    return Vector(
        vx * m[0][0] + vy * m[1][0] + vz * m[2][0] + m[3][0],
        vx * m[0][1] + vy * m[1][1] + vz * m[2][1] + m[3][1],
        vx * m[0][2] + vy * m[1][2] + vz * m[2][2] + m[3][2]
    ) / w;
}


}


