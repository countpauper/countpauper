#pragma once

namespace Engine
{

struct Vector;
struct Matrix;

struct Quaternion
{
    Quaternion() = default;
    Quaternion(double x, double y, double z, double w);
    Quaternion(const Vector& axis, double angle);
    void Normalize();
    bool IsNormalized() const;
    Quaternion Normalized() const;
    Quaternion Conjugate() const;
    Quaternion operator-();

    Matrix Matrix() const;
    double x, y, z, w;
    static Quaternion Identity();
private:
    double SquareMagnitude() const;
    static const double precision; // 1e-12;
};

Vector operator*(const Quaternion& q, const Vector& v);
Quaternion operator*(const Quaternion& a, const Quaternion & b);

} // ::Engine
