#pragma once

#include <ostream>
#include <istream>
#include <limits>
#include <tuple>

namespace Engine
{

struct Coordinate;
struct Matrix;
struct Line;
struct Plane;
struct Quaternion;
struct Position;
class Orientation;

class Vector
{
public:
    Vector() = default;
    constexpr Vector(double x, double y, double z) :
        x(x),
        y(y),
        z(z)
    {
    }
    explicit Vector(Coordinate c);
    explicit Vector(const Position& p);
    explicit Vector(const Line& l);
    explicit Vector(const Plane& p);
    explicit operator bool() const;
    double Length() const;
    double LengthSquared() const;
    double Axis(const Orientation& axis) const;
    constexpr double X() const { return x; }
    constexpr double Y() const { return y; }
    constexpr double Z() const { return z; }
    constexpr std::tuple<double, double, double> Coefficients() const { return std::make_tuple(x, y, z); }

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

    static const Vector zero;
    static const Vector XAxis;
    static const Vector YAxis;
    static const Vector ZAxis;
private:
    double x;
    double y;
    double z;

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
