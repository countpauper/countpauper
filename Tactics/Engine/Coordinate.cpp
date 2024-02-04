#include "Coordinate.h"
#include "Vector.h"
#include "Matrix.h"
#include "from_string.h"
#include <ostream>
#include <istream>
#include "Utils.h"

namespace Engine
{


const Coordinate Coordinate::origin { 0,0,0 };

Coordinate& Coordinate::operator+=(const Vector& v)
{
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
}

Coordinate& Coordinate::operator-=(const Vector& v)
{
    x -= v.x;
    y -= v.y;
    z -= v.z;
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


Coordinate operator*(const Coordinate& c, double factor)
{
    return Coordinate(c) *= factor;
}

Coordinate operator+(const Coordinate& c, const Vector& v)
{
    return Coordinate(c) += v;
}
Coordinate operator-(const Coordinate& c, const Vector& v)
{
    return Coordinate(c) -= v;
}


Coordinate operator*(const Matrix& m, const Coordinate& v)
{
    double w = v.x * m[0][3] + v.y * m[1][3] + v.z * m[2][3] + m[3][3];
    return Coordinate(
        (v.x * m[0][0] + v.y * m[1][0] + v.z * m[2][0] + m[3][0]) / w,
        (v.x * m[0][1] + v.y * m[1][1] + v.z * m[2][1] + m[3][1]) / w,
        (v.x * m[0][2] + v.y * m[1][2] + v.z * m[2][2] + m[3][2]) / w
    );
}

std::ostream& operator<<(std::ostream& s, const Coordinate& coordinate)
{
    s << "(" << coordinate.x << "," << coordinate.y << "," << coordinate.z << ")";
    return s;
}


std::istream& operator>>(std::istream& s, Coordinate& coordinate)
{
    std::string str;
    
    s >> Engine::Skip(Engine::whitespace) >> Engine::ReadChar('(') >> Engine::ReadUntil(')', str);

    auto value_strings= Split(str, ',');
    if (value_strings.size()>3)
        throw std::runtime_error("Invalid vector format, too many dimensions");
    if (value_strings.size() >= 1)
        coordinate.x = from_string<double>(value_strings[0]);
    else
        coordinate.x = 0;
    if (value_strings.size() >= 2)
        coordinate.y = from_string<double>(value_strings[1]);
    else
        coordinate.y = 0;
    if (value_strings.size() >= 3)
        coordinate.z = from_string<double>(value_strings[2]);
    else
        coordinate.z = 0;
    return s;
}


}


