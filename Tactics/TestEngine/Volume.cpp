#include "pch.h"
#include "Volume.h"
#include "Mesh.h"
#include "Matrix.h"

namespace Engine::Test
{

TEST(Volume, Point)
{
    Point p(Coordinate(1, 1, 1));
    EXPECT_EQ(0, p.GetBoundingBox().Volume());
    EXPECT_EQ(1, p.Distance(Coordinate(0, 1, 1)));
}

TEST(Volume , Sphere)
{
    Sphere s(Coordinate(1, 1, 1), 1);
    EXPECT_EQ(8, s.GetBoundingBox().Volume());
    EXPECT_TRUE(s.GetBoundingBox().Contains(Coordinate(0, 1, 1)));
    EXPECT_EQ(0, s.Distance(Coordinate(0, 1, 1)));
    EXPECT_EQ(-1, s.Distance(Coordinate(1, 1, 1)));
    EXPECT_EQ(2, s.Distance(Coordinate(1, 1, 4)));
}


TEST(Volume, AABox)
{
    AABox b(Coordinate(-1.0, 0.0, -1.0), Coordinate(2, 1, 0));
    EXPECT_EQ(3*1*1, b.GetBoundingBox().Volume());
    EXPECT_TRUE(b.GetBoundingBox().Contains(Coordinate(0, 0.5, -0.5)));
    EXPECT_EQ(0, b.Distance(Coordinate(0, 1, -1)));
    EXPECT_EQ(-0.5, b.Distance(Coordinate(0, 0.5, -0.5)));
    EXPECT_EQ(1, b.Distance(Coordinate(1, 1, 1)));
}

TEST(Volume, InfiniteAABox)
{
    AABox b(Range<double>::infinity(), Range<double>::infinity(), Range<double>(-2,0));
    EXPECT_TRUE(std::isinf(b.GetBoundingBox().Volume()));
    EXPECT_EQ(2, b.Distance(Coordinate(0, 0, 2)));
    EXPECT_EQ(0, b.Distance(Coordinate(0, 0, 0)));
    EXPECT_EQ(-1, b.Distance(Coordinate(0, 0, -1)));
}


TEST(Volume, Box)
{
    Box b(Vector(2, 3, 4));
    b *= Matrix::Translation(Vector(1, 1.5, 2)); // Box is centered around 0,0,0
    b *= Matrix::Translation(Vector(1, -1, 0 )); // [1,3],[-1,2],[0,4]
    double precision = 10e-12;
    EXPECT_NEAR(24, b.GetBoundingBox().Volume(), precision);
    // On corners
    EXPECT_NEAR(0, b.Distance(Coordinate(1, -1, 4)), precision);
    EXPECT_NEAR(0, b.Distance(Coordinate(3, 2, 0)), precision);
    // On faces
    EXPECT_NEAR(0, b.Distance(Coordinate(2, -1, 0)), precision);
    EXPECT_NEAR(0, b.Distance(Coordinate(3, 0, 4)), precision);
    // Outside
    EXPECT_NEAR(1, b.Distance(Coordinate(0, 0, 2)), precision);
    EXPECT_NEAR(2, b.Distance(Coordinate(2, 4, 3)), precision);
    // Inside
    EXPECT_NEAR(-1, b.Distance(Coordinate(2, 0, 2)), precision);
}

TEST(Volume, NulCylinder)
{
    Cylinder c(Line(Coordinate(0, 0, 0), Coordinate(0, 0, 0)), 0, 0);
    EXPECT_FALSE(c.GetBoundingBox().Volume());
    EXPECT_EQ(0, c.Distance(Coordinate(0, 0, 0)));
    EXPECT_EQ(1.0, c.Distance(Coordinate(1, 0, 0)));
}

TEST(Volume, AxisAlignedCylinder)
{
    Cylinder c(Line(Coordinate(0, 0, 0), Coordinate(1, 0, 0)), 1, 1);
    EXPECT_DOUBLE_EQ(4.0, c.GetBoundingBox().Volume()); // 1x2x2
    EXPECT_EQ(0, c.Distance(Coordinate(0, 0, 0)));
    EXPECT_EQ(0.0, c.Distance(Coordinate(1, 0, 0)));
    EXPECT_EQ(0.0, c.Distance(Coordinate(0, 1, 0)));
    EXPECT_EQ(0.0, c.Distance(Coordinate(0, 0, 1)));
    EXPECT_EQ(1, c.Distance(Coordinate(-1, 0, 0)));
    EXPECT_EQ(1, c.Distance(Coordinate( 0, -2, 0)));
    EXPECT_EQ(1, c.Distance(Coordinate(0, 0, 2)));
    EXPECT_DOUBLE_EQ(sqrt(2) - 1.0, c.Distance(Coordinate(0.5, 1, 1)));
    EXPECT_DOUBLE_EQ(sqrt(2) - 1.0, c.Distance(Coordinate(0, 1, 1)));
    EXPECT_DOUBLE_EQ(sqrt(2) - 1.0, c.Distance(Coordinate(1, 1, 1)));
}

TEST(Volume, EllipseCylinder)
{
    Cylinder flat(Line(Coordinate(0, 0, 0), Coordinate(1, 0, 0)), 1, 0.0);
    EXPECT_DOUBLE_EQ(0.0, flat.GetBoundingBox().Volume()); 
    EXPECT_EQ(0.0, flat.Distance(Coordinate(0.5, 0.0, 0.0)));
    EXPECT_EQ(1.0, flat.Distance(Coordinate(0.5, 0.0, 1.0)));
    EXPECT_EQ(0.0, flat.Distance(Coordinate(0.5, 1.0, 0.0)));

}

}
