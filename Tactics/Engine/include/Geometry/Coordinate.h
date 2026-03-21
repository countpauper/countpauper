#pragma once
#include <sstream>
namespace Engine
{

struct Vector;
struct Matrix;
class Orientation;

class Coordinate final
{
public:
    Coordinate() = default;
    constexpr Coordinate(double x, double y, double z) :
        x(x),
        y(y),
        z(z)
    {
    }
    double Distance(Coordinate other) const;

    constexpr double X() const { return x; }
    constexpr double Y() const { return y; }
    constexpr double Z() const { return z; }
    double Axis(const Orientation& axis) const;
    constexpr std::tuple<double, double, double> Coefficients() const { return std::make_tuple(x, y, z); }

    Coordinate& operator+=(Vector v);
    Coordinate& operator-=(Vector v);
    Coordinate& operator*=(double factor);
    Coordinate& operator*=(const Matrix& transformation);
    bool operator==(const Coordinate& other) const;
    void Render() const;

    static const Coordinate origin;
private:
    double x;
    double y;
    double z;
};

Coordinate operator*(Coordinate c, double factor);
Coordinate operator+(Coordinate c, Vector v);
Coordinate operator-(Coordinate c, Vector v);
Coordinate operator*(const Matrix& m, Coordinate v);

std::ostream& operator<<(std::ostream& s, Coordinate coordinate);
std::istream& operator>>(std::istream& s, Coordinate& coordinate);

Vector operator-(Coordinate a, Coordinate b);

} // ::Engine
