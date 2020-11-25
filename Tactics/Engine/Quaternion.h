#pragma once

namespace Engine
{

struct Vector;
struct Matrix;

struct Quaternion
{
    Quaternion() = default;
    Quaternion(const Vector& axis, double angle);
    void Normalize();
    bool IsNormalized() const;
    Quaternion Normalized() const;

    Matrix Matrix() const;
    double x, y, z, w;
private:
    double SquareMagnitude() const;
    static const double precision; // 1e-12;
};

Vector operator*(const Quaternion& q, const Vector& v);

} // ::Engine
