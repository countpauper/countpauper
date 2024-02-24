#pragma once
#include <sstream>
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
    double Distance(Coordinate other) const;

    Coordinate& operator+=(Vector v);
    Coordinate& operator-=(Vector v);
    Coordinate& operator*=(double factor);
    Coordinate& operator*=(const Matrix& transformation);
    bool operator==(const Coordinate& other) const;
    void Render() const;

    double x;
    double y;
    double z;
    static const Coordinate origin;
};

Coordinate operator*(Coordinate c, double factor);
Coordinate operator+(Coordinate c, Vector v);
Coordinate operator-(Coordinate c, Vector v);
Coordinate operator*(const Matrix& m, Coordinate v);

std::ostream& operator<<(std::ostream& s, Coordinate coordinate);
std::istream& operator>>(std::istream& s, Coordinate& coordinate);

Vector operator-(Coordinate a, Coordinate b);

} // ::Engine
