#include "Quaternion.h"
#include "Vector.h"
#include "Matrix.h"
#include <cmath>
#include <limits>

namespace Engine
{

const double Quaternion::precision = 1e-12;

Quaternion::Quaternion(double x, double y, double z, double w) :
    x(x),
    y(y),
    z(z),
    w(w)
{
}


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


Quaternion Quaternion::Conjugate() const
{
    return Quaternion(-x, -y, -z, w);
}

Quaternion Quaternion::operator-()
{
    return Conjugate();
}

bool Quaternion::operator==(const Quaternion& o) const
{
    // FOr the same orientation, it could also be x==-o.x && y==-o.y and so on, but for the same rotation the sign has to match too
    return x == o.x && y == o.y && z == o.z && w == o.w;
}

bool Quaternion::IsNormalized() const
{
    auto sqrMag = SquareMagnitude();
    return (std::abs(1 - sqrMag) < precision);

}

Matrix Quaternion::AsMatrix() const
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

Quaternion Quaternion::Identity()
{
    return Quaternion(0, 0, 0, 1);
}

Quaternion Quaternion::Shortest(const Vector& a, const Vector& b)
{
    Vector an = a.Normal();
    Vector bn = b.Normal();
    auto dot = an.Dot(bn);
    if (dot >= 1.0 - std::numeric_limits<double>::epsilon())
        return Quaternion::Identity();

    auto cross = an.Cross(bn);
    return Quaternion{ cross.x, cross.y, cross.z, 1+dot }.Normalized();
}

Vector operator*(const Quaternion& q, const Vector& v)
{
/*    //gamedev.stackexchange.com/questions/28395/rotating-vector3-by-a-quaternion

    Vector u(q.x, q.y, q.z);
    double s = q.w;
    return u * 2.0 * u.Dot(v) + v * (s*s - u.Dot(u)) + u.Cross(v) * s * 2.0;
*/

    return Vector(
        q.w * q.w*v.x   + 2 * q.y*q.w*v.z  - 2 * q.z*q.w*v.y + q.x * q.x*v.x     + 2 * q.y*q.x*v.y + 2 * q.z*q.x*v.z - q.z * q.z*v.x - q.y * q.y*v.x,
        2 * q.x*q.y*v.x + q.y * q.y*v.y    + 2 * q.z*q.y*v.z + 2 * q.w*q.z*v.x   - q.z * q.z*v.y   + q.w * q.w*v.y   - 2 * q.x*q.w*v.z - q.x * q.x*v.y,
        2 * q.x*q.z*v.x + 2 * q.y*q.z*v.y  + q.z * q.z*v.z   - 2 * q.w*q.y*v.x   - q.y * q.y*v.z   + 2 * q.w*q.x*v.y - q.x * q.x*v.z + q.w * q.w*v.z);
}

Quaternion operator*(const Quaternion& q1, const Quaternion& q2)
{
    // http://www.euclideanspace.com/maths/algebra/realNormedAlgebra/quaternions/code/index.htm
    return Quaternion(
         q1.x * q2.w + q1.y * q2.z - q1.z * q2.y + q1.w * q2.x,
        -q1.x * q2.z + q1.y * q2.w + q1.z * q2.x + q1.w * q2.y,
         q1.x * q2.y - q1.y * q2.x + q1.z * q2.w + q1.w * q2.z,
        -q1.x * q2.x - q1.y * q2.y - q1.z * q2.z + q1.w * q2.w);
}

}
