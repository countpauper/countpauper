#pragma once

namespace Engine
{

struct Vector;

struct Coordinate
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

    double x;
    double y;
    double z;
};
Coordinate operator*(const Coordinate& c, double factor);
Coordinate operator+(const Coordinate& c, const Vector& v);
Coordinate operator-(const Coordinate& c, const Vector& v);

std::ostream& operator<<(std::ostream& s, const Coordinate& coordinate);
std::wostream& operator<<(std::wostream& s, const Coordinate& coordinate);
std::wistream& operator>>(std::wistream& s, Coordinate& coordinate);


Vector operator-(const Coordinate& a, const Coordinate& b);

} // ::Engine
