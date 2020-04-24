#pragma once

namespace Engine
{

struct Vector;
struct Matrix;

struct Quaternion
{
    Quaternion() = default;
    Quaternion(const Vector& axis, double angle);

    Matrix Matrix() const;
    double x, y, z, w;

};

Vector operator*(const Quaternion& q, const Vector& v);

} // ::Engine
