#include <gtest/gtest.h>
#include "Geometry/Quaternion.h"
#include "Geometry/Vector.h"
#include "Geometry/Angles.h"
#include "Geometry/Matrix.h"
#include "GTestGeometry.h"
#include <GL/gl.h>

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
    Quaternion xrot(Vector::X, PI * 0.5);
    Vector v(0, 2, 0);
    EXPECT_DOUBLE_EQ(2, (xrot*v).z);
}

TEST(Quaternion, Inverse)
{
    Quaternion xrot(Vector::X, PI * 0.5);
    Vector v(0, 2, 0);
    EXPECT_DOUBLE_EQ(-2, (-xrot*v).z);
}


TEST(Quaternion, Unit)
{
    EXPECT_TRUE(Quaternion(Vector::zero, 0).IsNormalized());
    EXPECT_TRUE(Quaternion(Vector::X, 0).IsNormalized());

    EXPECT_FALSE(Quaternion(Vector(1, -1, 0), 1.0).IsNormalized());
    EXPECT_TRUE(Quaternion(Vector(1, -1, 0), 1.0).Normalized().IsNormalized());

    EXPECT_TRUE(Quaternion(Vector::X, 0).AsMatrix().IsIdentity());
    EXPECT_TRUE(Quaternion(Vector::X, 1.0).IsNormalized());
    EXPECT_TRUE(Quaternion(Vector::X, 1.0).AsMatrix().IsAffine());
}

TEST(Quaternion, RightHandedRotation)
{
    Quaternion xrot(Vector::X, PI * 0.5);
    EXPECT_DOUBLE_EQ( 1, (xrot * Vector(1, 1, 1)).x);
    EXPECT_DOUBLE_EQ(-1, (xrot * Vector(1, 1, 1)).y);
    EXPECT_DOUBLE_EQ( 1, (xrot * Vector(1, 1, 1)).z);

    Quaternion yrot(Vector::Y, PI * 0.5);
    EXPECT_DOUBLE_EQ( 1, (yrot * Vector(1, 1, 1)).x);
    EXPECT_DOUBLE_EQ( 1, (yrot * Vector(1, 1, 1)).y);
    EXPECT_DOUBLE_EQ(-1, (yrot * Vector(1, 1, 1)).z);

    Quaternion zrot(Vector::Z, PI * 0.5);
    EXPECT_DOUBLE_EQ(-1, (zrot * Vector(1, 1, 1)).x);
    EXPECT_DOUBLE_EQ( 1, (zrot * Vector(1, 1, 1)).y);
    EXPECT_DOUBLE_EQ( 1, (zrot * Vector(1, 1, 1)).z);
}

TEST(Quaternion, Multiplication)
{
    Quaternion xrot(Vector::X, PI * 0.5);
    Quaternion yrot(Vector::Y, PI * 0.5);

    Vector v(1, 0, 0);
    // http://www.euclideanspace.com/maths/algebra/realNormedAlgebra/quaternions/transforms/index.htm
    // (q1*q2) * v =  (q2*v) * q1
    EXPECT_EQ((yrot*(xrot*v)), ((yrot*xrot)*v));
    EXPECT_EQ((xrot*(yrot*v)), ((xrot*yrot)*v));
}

TEST(Quaternion, Shortest)
{
    EXPECT_3D_EQ(Quaternion::Shortest(Vector::X, Vector::Z) * Vector::X, Vector::Z);
    EXPECT_3D_EQ(Quaternion::Shortest(Vector::X, Vector::Y) * Vector::X, Vector::Y);
    EXPECT_3D_EQ(Quaternion::Shortest(Vector::X, Vector::Z) * Vector(2, 0, 0), Vector(0, 0, 2));
    EXPECT_EQ(Quaternion::Shortest(Vector::Z, Vector::Z), Quaternion::Identity());

    Quaternion rot(Vector(0, 2, 3), PI*0.5);
    rot.Normalize();
    auto shortest = Quaternion::Shortest(Vector::X, rot * Vector::X);
    EXPECT_DOUBLE_EQ(shortest.x, rot.x);
    EXPECT_DOUBLE_EQ(shortest.y, rot.y);
    EXPECT_DOUBLE_EQ(shortest.z, rot.z);
    EXPECT_NEAR(shortest.w, rot.w, 1e-12);
}

}
