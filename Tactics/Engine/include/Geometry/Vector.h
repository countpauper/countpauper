#pragma once

#include <ostream>
#include <istream>

namespace Engine
{

struct Coordinate;
struct Matrix;
struct Line;
struct Plane;
struct Quaternion;

struct Vector
{
    Vector() = default;
    Vector(double x, double y, double z) :
        x(x),
        y(y),
        z(z)
    {
    }
    explicit Vector(Coordinate c);
    explicit Vector(const Line& l);
    explicit Vector(const Plane& p);

    operator bool() const;
    double Length() const;
    double LengthSquared() const;
    Vector Normal() const;
    bool IsNormalized() const;
    Vector Cross(Vector o) const;

    bool operator==(Vector o) const;
    Vector& operator*=(double factor);
    Vector& operator/=(double factor);
    Vector& operator+=(Vector v);
    Vector& operator-=(Vector v);
    Vector operator-() const;
    Vector& operator*=(const Matrix& m);
    Vector& operator*=(const Quaternion& m);

    double Dot(Vector v) const;

    double x;
    double y;
    double z;
    static const Vector zero;
    static const Vector X;
    static const Vector Y;
    static const Vector Z;

};
Vector operator*(Vector v, double factor);
Vector operator/(Vector v, double factor);
Vector operator*(Vector v, double factor);
Vector operator+(Vector a, Vector b);
Vector operator-(Vector a, Vector b);
Vector operator-(Coordinate a, Coordinate b);
Vector operator*(const Matrix& m, Vector v);

std::ostream& operator<<(std::ostream& s, Vector vector);
std::istream& operator>>(std::istream& s, Vector& vector);

} // ::Engine
