#include "Geometry/Coordinate.h"
#include "Geometry/Vector.h"
#include "Geometry/Matrix.h"
#include "Geometry/Orientation.h"
#include "Utility/from_string.h"
#include "Utility/String.h"
#include <GL/gl.h>
#include <ostream>
#include <istream>

namespace Engine
{



double Coordinate::Axis(const Orientation& axis) const
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

double Coordinate::Distance(Coordinate other) const
{
    return (*this - other).Length();
}

Coordinate& Coordinate::operator+=(Vector v)
{
    x += v.X();
    y += v.Y();
    z += v.Z();
    return *this;
}

Coordinate& Coordinate::operator-=(Vector v)
{
    x -= v.X();
    y -= v.Y();
    z -= v.Z();
    return *this;
}

Coordinate& Coordinate::operator*=(double factor)
{
    x *= factor;
    y *= factor;
    z *= factor;
    return *this;
}

Coordinate& Coordinate::operator*=(const Matrix& transformation)
{
    (*this) = transformation * (*this);
    return *this;
}

bool Coordinate::operator==(const Coordinate& other) const
{
    return x == other.x &&
        y == other.y &&
        z == other.z;
}


Coordinate operator*(Coordinate c, double factor)
{
    return Coordinate(c) *= factor;
}

Coordinate operator+(Coordinate c, Vector v)
{
    return Coordinate(c) += v;
}
Coordinate operator-(Coordinate c, Vector v)
{
    return Coordinate(c) -= v;
}

void Coordinate::Render() const
{
    glTranslated(x, y, z);
}

Coordinate operator*(const Matrix& m, Coordinate v)
{
    auto [vx, vy, vz] = v.Coefficients();

    double w = vx * m[0][3] + vy * m[1][3] + vz * m[2][3] + m[3][3];
    return Coordinate(
        (vx * m[0][0] + vy * m[1][0] + vz * m[2][0] + m[3][0]) / w,
        (vx * m[0][1] + vy * m[1][1] + vz * m[2][1] + m[3][1]) / w,
        (vx * m[0][2] + vy * m[1][2] + vz * m[2][2] + m[3][2]) / w
    );
}

std::ostream& operator<<(std::ostream& s, Coordinate coordinate)
{
    s << "(" << coordinate.X() << "," << coordinate.Y() << "," << coordinate.Z() << ")";
    return s;
}


std::istream& operator>>(std::istream& s, Coordinate& coordinate)
{
    std::string str;

    s >> Engine::Skip(Engine::whitespace) >> Engine::ReadChar('(') >> Engine::ReadUntil(')', str);

    auto value_strings= Split(str, ',');
    if (value_strings.size()>3)
        throw std::runtime_error("Invalid vector format, too many dimensions");
    double x,y,z;
    if (value_strings.size() >= 1)
        x = from_string<double>(value_strings[0]);
    else
        x = 0;
    if (value_strings.size() >= 2)
        y = from_string<double>(value_strings[1]);
    else
        y = 0;
    if (value_strings.size() >= 3)
        z = from_string<double>(value_strings[2]);
    else
        z = 0;
    coordinate = Coordinate(x,y,z);
    return s;
}


}


