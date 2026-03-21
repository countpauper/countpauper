#include "Geometry/Quaternion.h"
#include "Geometry/Vector.h"
#include "Geometry/Matrix.h"
#include <cmath>
#include <limits>
#include <cassert>

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
    x = axis.X() * sinHalfAngle;
    y = axis.Y() * sinHalfAngle;
    z = axis.Z() * sinHalfAngle;
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

Quaternion Quaternion::Shortest(const Vector& a, const Vector& b)
{
    assert(a.IsNormalized());
    assert(b.IsNormalized());
    auto dot = a.Dot(b);
    if (dot >= 1.0 - std::numeric_limits<double>::epsilon())
        return Quaternion::Identity;

    auto cross = a.Cross(b);
    return Quaternion{ cross.X(), cross.Y(), cross.Z(), 1+dot }.Normalized();
}

void Quaternion::Render() const
{
    AsMatrix().Render();
}

Vector operator*(const Quaternion& q, const Vector& v)
{
/*    //gamedev.stackexchange.com/questions/28395/rotating-vector3-by-a-quaternion

    Vector u(q.x, q.y, q.z);
    double s = q.w;
    return u * 2.0 * u.Dot(v) + v * (s*s - u.Dot(u)) + u.Cross(v) * s * 2.0;
*/
    auto [vx, vy, vz] = v.Coefficients();

    return Vector(
        q.w * q.w*vx   + 2 * q.y*q.w*vz  - 2 * q.z*q.w*vy + q.x * q.x*vx     + 2 * q.y*q.x*vy + 2 * q.z*q.x*vz - q.z * q.z*vx - q.y * q.y*vx,
        2 * q.x*q.y*vx + q.y * q.y*vy    + 2 * q.z*q.y*vz + 2 * q.w*q.z*vx   - q.z * q.z*vy   + q.w * q.w*vy   - 2 * q.x*q.w*vz - q.x * q.x*vy,
        2 * q.x*q.z*vx + 2 * q.y*q.z*vy  + q.z * q.z*vz   - 2 * q.w*q.y*vx   - q.y * q.y*vz   + 2 * q.w*q.x*vy - q.x * q.x*vz + q.w * q.w*vz);
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
