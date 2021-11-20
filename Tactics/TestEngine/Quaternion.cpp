#include "pch.h"
#include "Quaternion.h"
#include "Vector.h"
#include "Geometry.h"
#include "Matrix.h"

namespace Engine::Test
{


TEST(Quaternion, Identity)
{
    Quaternion identity = Quaternion::Identity();
    Vector v(1, 2, 3);
    EXPECT_EQ(v.x, (identity * v).x);
    EXPECT_EQ(v.y, (identity * v).y);
    EXPECT_EQ(v.z, (identity * v).z);
}

TEST(Quaternion, Rotate)
{
    Quaternion xrot(Vector(1, 0, 0), PI * 0.5);
    Vector v(0, 2, 0);
    EXPECT_DOUBLE_EQ(2, (xrot*v).z);
}

TEST(Quaternion, Inverse)
{
    Quaternion xrot(Vector(1, 0, 0), PI * 0.5);
    Vector v(0, 2, 0);
    EXPECT_DOUBLE_EQ(-2, (-xrot*v).z);
}


TEST(Quaternion, Unit)
{
    EXPECT_TRUE(Quaternion(Vector(0, 0, 0), 0).IsNormalized());
    EXPECT_TRUE(Quaternion(Vector(1, 0, 0), 0).IsNormalized());

    EXPECT_FALSE(Quaternion(Vector(1, -1, 0), 1.0).IsNormalized());
    EXPECT_TRUE(Quaternion(Vector(1, -1, 0), 1.0).Normalized().IsNormalized());

    EXPECT_TRUE(Quaternion(Vector(1, 0, 0), 0).Matrix().IsIdentity());
    EXPECT_TRUE(Quaternion(Vector(1, 0, 0), 1.0).IsNormalized());
    EXPECT_TRUE(Quaternion(Vector(1, 0, 0), 1.0).Matrix().IsAffine());
}

TEST(Quaternion, RightHandedRotation)
{
    Quaternion xrot(Vector(1, 0, 0), PI * 0.5);
    EXPECT_DOUBLE_EQ( 1, (xrot * Vector(1, 1, 1)).x);
    EXPECT_DOUBLE_EQ(-1, (xrot * Vector(1, 1, 1)).y);
    EXPECT_DOUBLE_EQ( 1, (xrot * Vector(1, 1, 1)).z);

    Quaternion yrot(Vector(0, 1, 0), PI * 0.5);
    EXPECT_DOUBLE_EQ( 1, (yrot * Vector(1, 1, 1)).x);
    EXPECT_DOUBLE_EQ( 1, (yrot * Vector(1, 1, 1)).y);
    EXPECT_DOUBLE_EQ(-1, (yrot * Vector(1, 1, 1)).z);

    Quaternion zrot(Vector(0, 0, 1), PI * 0.5);
    EXPECT_DOUBLE_EQ(-1, (zrot * Vector(1, 1, 1)).x);
    EXPECT_DOUBLE_EQ( 1, (zrot * Vector(1, 1, 1)).y);
    EXPECT_DOUBLE_EQ( 1, (zrot * Vector(1, 1, 1)).z);
}

TEST(Quaternion, Multiplication)
{
    Quaternion xrot(Vector(1, 0, 0), PI * 0.5);
    Quaternion yrot(Vector(0, 1, 0), PI * 0.5);
    
    Vector v(1, 0, 0);
    // http://www.euclideanspace.com/maths/algebra/realNormedAlgebra/quaternions/transforms/index.htm
    // (q1*q2) * v =  (q2*v) * q1 
    EXPECT_EQ((yrot*(xrot*v)), ((yrot*xrot)*v));
    EXPECT_EQ((xrot*(yrot*v)), ((xrot*yrot)*v));
    // transform x => y,  y => z and z =>x 
    auto mxy = (xrot*yrot).Matrix();
    auto mxy2 = Matrix::XRot(PI*0.5) * Matrix::YRot(PI*0.5);
    auto t = mxy2 * v;
    EXPECT_EQ(t, (xrot*yrot*v));
}


}