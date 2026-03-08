#include <gtest/gtest.h>
#include "Geometry/Triangle.h"
#include "Geometry/Vector.h"
#include "Geometry/Line.h"

namespace Engine::Test
{


TEST(Triangle, Surface)
{
    Triangle point(Coordinate(1, 1, 1), Coordinate(1, 1, 1), Coordinate(1, 1, 1));
    EXPECT_EQ(0,point.Surface());

    Triangle splinter(Coordinate(1, 1, 1), Coordinate(2, 2, 2), Coordinate(3, 3, 3));
    EXPECT_EQ(0, splinter.Surface());

    Triangle xyTriangle(Coordinate(1, 0, 0), Coordinate(3, 0, 0), Coordinate(1, 4, 0));
    EXPECT_EQ(4, xyTriangle.Surface());

    Triangle yzTriangle(Coordinate::origin, Coordinate(0, 1, 0), Coordinate(0, 0, 2));
    EXPECT_EQ(1, yzTriangle.Surface());

    Triangle xzTriangle(Coordinate::origin, Coordinate(0, 0, 3), Coordinate(0, 2, 1));
    EXPECT_EQ(3, xzTriangle.Surface());
}

TEST(Triangle, Distance)
{
    Triangle point(Coordinate(1, 1, 1), Coordinate(1, 1, 1), Coordinate(1, 1, 1));
    EXPECT_EQ(1, point.Distance(Coordinate(0, 1, 1)));
    EXPECT_EQ(1, point.Distance(Coordinate(1, 1, 2)));

    Triangle splinter(Coordinate(1, 1, 1), Coordinate(2, 2, 2), Coordinate(3, 3, 3));
    EXPECT_EQ(1, splinter.Distance(Coordinate(0, 1, 1)));
    EXPECT_EQ(2, splinter.Distance(Coordinate(3, 5, 3)));

    // right hand ccw (like default opengl): triangle is facing -z axis
    Triangle xyTriangle(Coordinate(1, 4, 0), Coordinate(3, 0, 0), Coordinate(1, 0, 0));
    EXPECT_LT(xyTriangle.Normal().z, 0);
    EXPECT_EQ(1, xyTriangle.Distance(Coordinate::origin)); // near first vertex
    EXPECT_EQ(3, xyTriangle.Distance(Coordinate(3, 0, -3))); // near second vertex, but in front
    EXPECT_EQ(2, xyTriangle.Distance(Coordinate(1, 6, 0))); // near third vertex
    EXPECT_EQ(0, xyTriangle.Distance(Coordinate(2, 0, 0))); // on edge
    EXPECT_EQ(1, xyTriangle.Distance(Coordinate(2, -1, 0))); // near edge
    EXPECT_EQ(0, xyTriangle.Distance(Coordinate(2, 2, 0))); // in triangle
    EXPECT_EQ(2, xyTriangle.Distance(Coordinate(2, 1, -2))); // in front of triangle
    EXPECT_EQ(-2, xyTriangle.Distance(Coordinate(2, 1, 2))); // behind triangle
    EXPECT_EQ(2, xyTriangle.Flipped().Distance(Coordinate(2, 1, 2))); // flipped triangle->flipped distance

    // triangle is facing +y axis
    Triangle xzTriangle(Coordinate::origin, Coordinate(0, 0, 3), Coordinate(2, 0, 1));
    EXPECT_GT(xzTriangle.Normal().y, 0);
    EXPECT_EQ(-1, xzTriangle.Distance(Coordinate(0, -1, 0))); // behind first vertex
    EXPECT_EQ(1, xzTriangle.Distance(Coordinate(3, 0, 1))); // in front of third vertex
    EXPECT_EQ(0, xzTriangle.Distance(Coordinate(0, 0, 2.5))); // on first edge
    EXPECT_EQ(-2, xzTriangle.Distance(Coordinate(0, -2, 2))); // near first edege
}

TEST(Triangle, Intersection)
{
    Triangle xyTriangle(Coordinate(0, 2, 0), Coordinate(3, 0, 0), Coordinate(1, 4, 0));
    EXPECT_NEAR(xyTriangle.Intersection(Line(Coordinate(2,1,2), Coordinate(2,1,-1))), 2.0, 0.01);
    EXPECT_NEAR(xyTriangle.Intersection(Line(Coordinate(2,1,-1), Coordinate(2,1,2))), -1, 0.01);
    EXPECT_EQ(xyTriangle.Intersection(Line(Coordinate(1,2,1.5), Coordinate(1,2,0))), 1.5);
    EXPECT_EQ(xyTriangle.Intersection(Line(Coordinate(1,2,0), Coordinate(1,2,-2))), 0.0);
    EXPECT_NEAR(xyTriangle.Intersection(Line(Coordinate(1,2,1), Coordinate(0,2,0))), sqrt(2), 0.00001);
    EXPECT_TRUE(std::isnan(xyTriangle.Intersection(Line(Coordinate(1,2,1), Coordinate(0,2,1))))) << "parallel line intersected the triangle";
    EXPECT_TRUE(std::isnan(xyTriangle.Intersection(Line(Coordinate(1,1,1), Coordinate(1,1,-1))))) << "line didn't miss the triangle";
}

}
