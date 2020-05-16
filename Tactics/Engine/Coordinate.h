#pragma once

namespace Engine
{

struct Vector;
struct Matrix;
struct Coordinate final
{
    Coordinate() = default;
    Coordinate(double x, double y, double z) :
        x(x),
        y(y),
        z(z)
    {
    }
    Coordinate& operator+=(const Vector& v);
    Coordinate& operator-=(const Vector& v);
    Coordinate& operator*=(double factor);
    Coordinate& operator*=(const Matrix& transformation);
    bool operator==(const Coordinate& other) const;

    double x;
    double y;
    double z;
    static const Coordinate zero;
};

Coordinate operator*(const Coordinate& c, double factor);
Coordinate operator+(const Coordinate& c, const Vector& v);
Coordinate operator-(const Coordinate& c, const Vector& v);
Coordinate operator*(const Matrix& m, const Coordinate& v);

std::ostream& operator<<(std::ostream& s, const Coordinate& coordinate);
std::wostream& operator<<(std::wostream& s, const Coordinate& coordinate);
std::wistream& operator>>(std::wistream& s, Coordinate& coordinate);


Vector operator-(const Coordinate& a, const Coordinate& b);

} // ::Engine
