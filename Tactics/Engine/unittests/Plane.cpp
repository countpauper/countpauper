#include <gtest/gtest.h>
#include "Geometry/Plane.h"
#include "Geometry/AxisAlignedBoundingBox.h"

namespace Engine::Test
{

TEST(Plane, Null)
{
    EXPECT_FALSE(Plane(Vector::zero, 1));
    EXPECT_FALSE(Plane(Vector::zero, 1));
    EXPECT_FALSE(Plane(Coordinate::origin, Coordinate::origin, Coordinate::origin));
    EXPECT_FALSE(Plane(Coordinate(1, 0, 0), Coordinate(1, 0, 0), Coordinate(1, 0, 0)));
    EXPECT_EQ(Plane(Vector::zero,0) , Plane(Coordinate(1, 0, 0), Coordinate(1, 0, 0), Coordinate(1, 0, 0)));
    EXPECT_FALSE(Plane::null);
    EXPECT_EQ(-Plane::null, Plane::null);
    EXPECT_FALSE(Plane::null.Normalized());
    EXPECT_THROW(Plane::null.Project(Coordinate(1,2,3)), std::runtime_error);
}

TEST(Plane, XY)
{
    EXPECT_TRUE(Plane::xy);
    EXPECT_EQ(Plane::xy, Plane(Coordinate::origin, Vector::X, Vector::Y));
    EXPECT_EQ(0, Plane::xy.Distance(Coordinate::origin));
    EXPECT_DOUBLE_EQ(1, Plane::xy.Distance(Coordinate(0, 0, 1)));
    EXPECT_DOUBLE_EQ(0, Plane::xy.Distance(Coordinate(1, 1, 0)));
    EXPECT_EQ(Coordinate::origin, Plane::xy.Project(Coordinate(0, 0, 1)));
    EXPECT_EQ(Coordinate(1, 1, 0), Plane::xy.Project(Coordinate(1, 1, 1)));
    EXPECT_TRUE(Plane::xy.Above(Coordinate(0, 0, 1)));
    EXPECT_FALSE((-Plane::xy).Above(Coordinate(0, 0, 1)));
    EXPECT_FALSE(Plane::xy.Above(Coordinate(0, 0, -1)));
}

TEST(Plane, XYIntersection)
{
    EXPECT_EQ(Plane::xy.Intersection(Line(Coordinate(0,0,2), Coordinate(0, 0, -6))).begin, 2);
    EXPECT_EQ(Plane::xy.Intersection(Line(Coordinate(0,0,-6), Coordinate(0, 0, 2))).begin, -6);
    EXPECT_FALSE(Plane::xy.Intersection(Line(Coordinate(0,1,2), Coordinate(1, 0, 2))));
    EXPECT_FALSE(Plane::xy.Intersection(Line(Coordinate(0,0,2), Coordinate(0, 0, 1))));
    EXPECT_FALSE(Plane::xy.Intersection(Line(Coordinate(0,0,-2), Coordinate(0, 0, -4))));
}


TEST(Plane, Diagonal)
{
    Plane plane(Coordinate(1, 0, 0), Coordinate(0, 1, 0), Coordinate(0, 0, 1));
    EXPECT_TRUE(plane);
    // as long as it's clockwise the same, the plane is the same
    EXPECT_EQ(Plane(Coordinate(0, 0, 1), Coordinate(1, 0, 0), Coordinate(0, 1, 0)), plane);
    EXPECT_EQ(Plane(Coordinate(0, 1, 0), Coordinate(0, 0, 1), Coordinate(1, 0, 0)), plane);
    // flipped direction is flipped plane
    EXPECT_EQ(Plane(Coordinate(1, 0, 0), Coordinate(0, 0, 1), Coordinate(0, 1, 0)), -plane);

    EXPECT_EQ(plane.Distance(Coordinate(1, 0, 0)), 0);
    EXPECT_EQ(plane.Distance(Coordinate(0, 1, 0)), 0);
    EXPECT_EQ(plane.Distance(Coordinate(0, 0, 1)), 0);
    EXPECT_DOUBLE_EQ(-1.0 / sqrt(3), plane.Distance(Coordinate::origin));
    EXPECT_EQ(plane.Project(Coordinate::origin), Coordinate(1.0 / 3.0, 1.0 / 3.0, 1.0 / 3.0));
}

TEST(Plane, NotNormalXYAt1)
{
    Plane xy2(Coordinate(1,1,1), Vector(0, 0, 2));
    EXPECT_TRUE(xy2.Above(Coordinate(1, 1, 1.1)));
    EXPECT_FALSE(xy2.Above(Coordinate(0, 0, 0.9)));
    EXPECT_DOUBLE_EQ(xy2.Distance(Coordinate(1,1,2)), 1);
    EXPECT_DOUBLE_EQ(xy2.Distance(Coordinate::origin), -1);
    EXPECT_DOUBLE_EQ(xy2.Distance(Coordinate(1,1,-1)), -2);
    EXPECT_EQ(Coordinate(0, 0, 1), xy2.Project(Coordinate::origin));
    EXPECT_EQ(Coordinate(1, 1, 1), xy2.Project(Coordinate(1, 1, 1)));
    EXPECT_EQ(Coordinate(-2,-3, 1), xy2.Project(Coordinate(-2, -3, -4)));
    // bounding box should be inf, inf, 1 ... inf
    EXPECT_TRUE(xy2.GetBoundingBox().x.infinite());
    EXPECT_TRUE(xy2.GetBoundingBox().y.infinite());
    EXPECT_FALSE(xy2.GetBoundingBox().Contains(Coordinate(1, 1, 1.1)));
    EXPECT_TRUE(xy2.GetBoundingBox().Contains(Coordinate(0, 0, 0.9)));

    EXPECT_EQ(xy2.Intersection(Line(Coordinate(0,0,2), Coordinate(0, 0, 0))).begin, 1);

}

TEST(Plane, NotNormalDiagonal)
{
    Plane abnormal(Coordinate(1,0,0), Vector(-2, 2, 0), Vector(0, 2, -2));
    EXPECT_TRUE(abnormal);
    EXPECT_EQ(abnormal.Normalized(), abnormal);
    EXPECT_EQ(abnormal.Distance(Coordinate(1, 0, 0)), 0);
    EXPECT_EQ(abnormal.Distance(Coordinate(-1, 2, 0)), 0);
    EXPECT_EQ(abnormal.Distance(Coordinate(1, 2, -2)), 0);
    EXPECT_EQ(abnormal.Project(Coordinate::origin), Coordinate(1.0/3.0, 1.0/3.0, 1.0/3.0));
    EXPECT_TRUE(abnormal.GetBoundingBox().x.infinite());
    EXPECT_TRUE(abnormal.GetBoundingBox().y.infinite());
    EXPECT_TRUE(abnormal.GetBoundingBox().z.infinite());

}

}
