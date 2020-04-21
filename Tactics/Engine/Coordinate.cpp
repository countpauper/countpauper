#include "stdafx.h"
#include "Coordinate.h"
#include "from_string.h"
#include <ostream>
#include "Utils.h"

namespace Engine
{


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

Coordinate& Coordinate::operator*=(float factor)
{
    x *= factor;
    y *= factor;
    z *= factor;
    return *this;
}

Coordinate operator*(const Coordinate& c, float factor)
{
    return Coordinate(c) *= factor;
}
Coordinate operator*(const Coordinate& c, double factor)
{
    return c * float(factor);
}
Coordinate operator+(const Coordinate& c, const Vector& v)
{
    return Coordinate(c) += v;
}
Coordinate operator-(const Coordinate& c, const Vector& v)
{
    return Coordinate(c) -= v;
}

std::ostream& operator<<(std::ostream& s, const Coordinate& coordinate)
{
    s << "(" << coordinate.x << "," << coordinate.y << "," << coordinate.z << ")";
    return s;
}

std::wostream& operator<<(std::wostream& s, const Coordinate& coordinate)
{
    s << L"(" << coordinate.x << L"," << coordinate.y << L"," << coordinate.z << L")";
    return s;

}

std::wistream& operator>>(std::wistream& s, Coordinate& coordinate)
{
    std::wstring str;
    s >> str;
    if (str[0] != L'(')
        throw std::runtime_error("Invalid coordinate format, missing (");
    if (str[str.size()-1] != L')')
        throw std::runtime_error("Invalid coordinate format, missing )");
    str = str.substr(1, str.size() - 2);
    auto value_strings= Split(str, L',');
    if (value_strings.size()>3)
        throw std::runtime_error("Invalid coordinate format, too many dimensions");
    if (value_strings.size() >= 1)
        coordinate.x = from_string<float>(value_strings[0]);
    else
        coordinate.x = 0;
    if (value_strings.size() >= 2)
        coordinate.y = from_string<float>(value_strings[1]);
    else
        coordinate.y = 0;
    if (value_strings.size() >= 3)
        coordinate.z = from_string<float>(value_strings[2]);
    else
        coordinate.z = 0;
    return s;
}


Vector& Vector::operator=(const Vector& o)
{
    x = o.x;
    y = o.y;
    z = o.z;
    return *this;
}
Vector::operator bool() const
{
    return x != 0 || y != 0 || z != 0;
}
double Vector::Length() const
{
    return sqrt(LengthSquared());
}
double Vector::LengthSquared() const
{
    return x * x + y * y + z * z;
}

bool Vector::operator==(const Vector& o) const
{
    return x == o.x && y == o.y && z == o.z;
}

Vector& Vector::operator*=(float factor)
{
    x *= factor;
    y *= factor;
    z *= factor;
    return *this;
}

Vector& Vector::operator+=(const Vector& v)
{
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
}

Vector& Vector::operator-=(const Vector& v)
{
    x -= v.x;
    y -= v.y;
    z -= v.z;
    return *this;
}

double Vector::Dot(const Vector& v) const
{
    return double(x) * v.x + double(y) * v.y + double(z) * v.z; 
}

Vector operator*(const Vector& v, float factor)
{
    return Vector(v) *= factor;
}

Vector operator*(const Vector& v, double factor)
{
    return v * float(factor);
}

Vector operator+(const Vector& a, const Vector& b)
{
    return Vector(a) += b;
}

Vector operator-(const Vector& a, const Vector& b)
{
    return Vector(a) -= b;
}

Vector operator-(const Coordinate& a, const Coordinate& b)
{
    return Vector(a.x - b.x, a.y - b.y, a.z - b.z);
}
}


