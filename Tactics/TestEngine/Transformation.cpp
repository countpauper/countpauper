#include "pch.h"
#include "Matrix.h"
#include "Quaternion.h"
#include "Vector.h"
#include "Geometry.h"

namespace Engine::Test
{


TEST(Matrix, Identity)
{
    Matrix identity = Matrix::Identity();
    Vector v(1, 2, 3);
    EXPECT_TRUE(identity.IsIdentity());
    EXPECT_EQ(v.x, (identity * v).x);
    EXPECT_EQ(v.y, (identity * v).y);
    EXPECT_EQ(v.z, (identity * v).z);
}

TEST(Matrix, Scale)
{
    auto matrix = Matrix::Scale(Vector(0.5, 1, 2));
    EXPECT_EQ(Vector(0.5, 1, 2), matrix.Scale());
    EXPECT_EQ(Vector(1, 3, 1), matrix*Vector(2, 3, 0.5));

    EXPECT_TRUE(matrix.NormalScale().IsIdentity());
}

TEST(Matrix, Multiplication)
{
    Matrix l{ 1,4, 0,0, 2, 5, 0,0, 3,6,0,0, 0,0,0,0 };
    Matrix r{ 7,9,11,0, 8,10,12,0, 0,0,0,0, 0,0,0,0 };

    auto o = l * r;
    EXPECT_EQ(58, o[0][0]);
    EXPECT_EQ(64, o[1][0]);
    EXPECT_EQ(0, o[2][0]);
    EXPECT_EQ(139, o[0][1]);
    EXPECT_EQ(154, o[1][1]);
    EXPECT_EQ(0, o[0][2]);

    l *= r; 
    EXPECT_EQ(58,  l[0][0]);
    EXPECT_EQ(64,  l[1][0]);
    EXPECT_EQ(139, l[0][1]);
    EXPECT_EQ(154, l[1][1]);
}



TEST(Matrix, InverseAffine)
{
    Quaternion xrot(Vector(1, 0, 0), float(PI / 2));
    auto matrix = xrot.Matrix()*Matrix::Translation(Vector(2, -3, 4));

    Vector v = Vector(1, -2, 0.5);
    auto v_transformed = matrix * v;
    auto inverse = matrix.Inverse() * v_transformed;
    EXPECT_NEAR(inverse.x, v.x, 1e-6);
    EXPECT_NEAR(inverse.y, v.y, 1e-6);
    EXPECT_NEAR(inverse.z, v.z, 1e-6);
    
    auto matrix2 = Matrix::Scale(Vector(0.2f,0.3f,0.4f)) * 
        Matrix::Translation(Vector(-2, 3, -4)) * 
        Quaternion(Vector(1, 0.5f, 0.5f), -0.3f).Matrix();
    v_transformed = matrix * v;
    inverse = matrix.Inverse() * v_transformed;
    EXPECT_NEAR(inverse.x, v.x, 1e-6);
    EXPECT_NEAR(inverse.y, v.y, 1e-6);
    EXPECT_NEAR(inverse.z, v.z, 1e-6);
}

TEST(Matrix, InversePerspective)
{
    auto matrix = Matrix::Perspective(1,2);
    EXPECT_FALSE(matrix.IsAffine());

    auto v = matrix * Vector(1, -2, 0.5);
    auto inverse = matrix.Inverse() * v;
    auto identity = matrix * matrix.Inverse();
    ASSERT_TRUE(identity.IsIdentity());
    EXPECT_NEAR(inverse.x, 1, 1e-6);
    EXPECT_NEAR(inverse.y, -2, 1e-6);
    EXPECT_NEAR(inverse.z, 0.5, 1e-6);
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
    Quaternion xrot(Vector(1, 0, 0), float(PI / 2));
    EXPECT_NEAR( 1, (xrot * Vector(1, 1, 1)).x, 1e-6);
    EXPECT_NEAR(-1, (xrot * Vector(1, 1, 1)).y, 1e-6);
    EXPECT_NEAR( 1, (xrot * Vector(1, 1, 1)).z, 1e-6);

    Quaternion yrot(Vector(0, 1, 0), float(PI / 2));
    EXPECT_NEAR( 1, (yrot * Vector(1, 1, 1)).x, 1e-6);
    EXPECT_NEAR( 1, (yrot * Vector(1, 1, 1)).y, 1e-6);
    EXPECT_NEAR(-1, (yrot * Vector(1, 1, 1)).z, 1e-6);

    Quaternion zrot(Vector(0, 0, 1), float(PI / 2));
    EXPECT_NEAR(-1, (zrot * Vector(1, 1, 1)).x, 1e-6);
    EXPECT_NEAR( 1, (zrot * Vector(1, 1, 1)).y, 1e-6);
    EXPECT_NEAR( 1, (zrot * Vector(1, 1, 1)).z, 1e-6);
}

}