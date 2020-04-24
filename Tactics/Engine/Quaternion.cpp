#include "stdafx.h"
#include "Quaternion.h"
#include "Vector.h"
#include "Matrix.h"

namespace Engine
{

Quaternion::Quaternion(const Vector& axis, double angle) : 
    w(cos(angle/2))
{
    double sinHalfAngle = sin(angle / 2);
    x = axis.x * sinHalfAngle;
    y = axis.y * sinHalfAngle;
    z = axis.z * sinHalfAngle;
}

Matrix Quaternion::Matrix() const
{   // https://www.cprogramming.com/tutorial/3d/quaternions.html
    double wsqr = w * w; // can be optimized more probably by precomupting 2* xsqr and 2xy 2xz and so on
    double xsqr = x * x;
    double ysqr = y * y; 
    double zsqr = z * z;
    Engine::Matrix m;
/*  // still row major
    m[0][0] = wsqr + xsqr - ysqr - zsqr;    m[0][1] = 2 * x*y - 2 * w*z;            m[0][2] = 2 * x*z + 2 * w*y;    m[0][3] = 0;
    m[1][0] = 2 * x*y + 2 * w*z;            m[1][1] = wsqr - xsqr + ysqr - zsqr;    m[1][2] = 2 * y*z + 2 * w*x;    m[1][3] = 0;
    m[2][0] = 2 * x*z - 2 * w*y;            m[2][1] = 2 * y*z - 2 * w*x;            m[2][2] = wsqr-xsqr-ysqr+zsqr;  m[2][3] = 0;
    m[3][0] = 0;                            m[3][1] = 0;                            m[3][2] = 0;                    m[3][3] = 1;
*/
    m[0][0] = 1 - (2 * ysqr) - (2 * zsqr);  m[1][0] = 2 * x*y - 2 * w*z;            m[2][0] = 2 * x*z + 2 * w*y;            m[3][0] = 0;
    m[0][1] = 2 * x*y + 2 * w*z;            m[1][1] = 1 - (2 * xsqr) - (2 * zsqr);  m[2][1] = 2 * y*z + 2 * w*x;            m[3][1] = 0;
    m[0][2] = 2 * x*z - 2 * w*y;            m[1][2] = 2 * y*z - 2 * w*x;            m[2][2] = 1 - (2 * xsqr) - (2 * ysqr);  m[3][2] = 0;
    m[0][3] = 0;                            m[1][3] = 0;                            m[2][3] = 0;                            m[3][3] = 1;
    return m;
}



Vector operator*(const Quaternion& q, const Vector& v)
{
    return q.Matrix() * v;
}


}
