#pragma once

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
    Vector(const Vector& v) = default;
    explicit Vector(const Coordinate& c);
    explicit Vector(const Line& l);
    explicit Vector(const Plane& p);

    Vector& operator=(const Vector& o);
    operator bool() const;
    double Length() const;
    double LengthSquared() const;
    Vector Normal() const;
    Vector Cross(const Vector& o) const;

    bool operator==(const Vector& o) const;
    Vector& operator*=(double factor);
    Vector& operator/=(double factor);
    Vector& operator+=(const Vector& v);
    Vector& operator-=(const Vector& v);
    Vector operator-() const;
    Vector& operator*=(const Matrix& m);
    Vector& operator*=(const Quaternion& m);

    double Dot(const Vector& v) const;

    double x;
    double y;
    double z;
};
Vector operator*(const Vector& v, double factor);
Vector operator/(const Vector& v, double factor);
Vector operator*(const Vector& v, double factor);
Vector operator+(const Vector& a, const Vector& b);
Vector operator-(const Vector& a, const Vector& b);
Vector operator-(const Coordinate& a, const Coordinate& b);
Vector operator*(const Matrix& m, const Vector& v);

std::ostream& operator<<(std::ostream& s, const Vector& vector);
std::wostream& operator<<(std::wostream& s, const Vector& vector);
std::wistream& operator>>(std::wistream& s, Vector& vector);

} // ::Engine
