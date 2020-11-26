#include "stdafx.h"
#include "Quaternion.h"
#include "Vector.h"
#include "Matrix.h"

namespace Engine
{

const double Quaternion::precision = 1e-12;

Quaternion::Quaternion(const Vector& axis, double angle) : 
    w(cos(angle/2))
{
    double sinHalfAngle = sin(angle / 2);
    x = axis.x * sinHalfAngle;
    y = axis.y * sinHalfAngle;
    z = axis.z * sinHalfAngle;
}

double Quaternion::SquareMagnitude() const
{
    return x * x + y * y + z * z + w * w;
}

void Quaternion::Normalize()
{
    auto sqrMag = SquareMagnitude();
    if (std::abs(1 - sqrMag) < precision)
        return;
    if (std::abs(sqrMag) < std::numeric_limits<double>::epsilon())
        return;
    double mag = sqrt(sqrMag);
    x /= mag;
    y /= mag;
    z /= mag;
    w /= mag;
}

Quaternion Quaternion::Normalized() const
{
    Quaternion o(*this);
    o.Normalize();
    return o;
}

bool Quaternion::IsNormalized() const
{
    auto sqrMag = SquareMagnitude();
    return (std::abs(1 - sqrMag) < precision);

}

Matrix Quaternion::Matrix() const
{   // https://en.wikipedia.org/wiki/Quaternions_and_spatial_rotation#Quaternion-derived_rotation_matrix
    double wsqr = w * w; // can be optimized more probably by precomupting 2* xsqr and 2xy 2xz and so on
    double xsqr = x * x;
    double ysqr = y * y; 
    double zsqr = z * z;
    // column major like opengl  m[col][row]
    return Engine::Matrix{
         1 - (2 * ysqr) - (2 * zsqr),   2 * x*y + 2 * w*z,             2 * x*z - 2 * w*y,             0,
         2 * x*y - 2 * w*z,             1 - (2 * xsqr) - (2 * zsqr),   2 * y*z + 2 * w*x,             0,
         2 * x*z + 2 * w*y,             2 * y*z - 2 * w*x,             1 - (2 * xsqr) - (2 * ysqr),   0,
         0,                             0,                             0,                             1 };
}



Vector operator*(const Quaternion& q, const Vector& v)
{
    return q.Matrix() * v;
}


}
