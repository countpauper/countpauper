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
}
