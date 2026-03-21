#include <gtest/gtest.h>
#include "Geometry/AxisAlignedPlane.h"
#include "Geometry/AxisAlignedBoundingBox.h"
#include "Geometry/Plane.h"

namespace Engine::Test
{

TEST(AxisAlignedPlane, Null)
{
    EXPECT_FALSE(AxisAlignedPlane::null);
    EXPECT_EQ(-AxisAlignedPlane::null, AxisAlignedPlane::null);
    EXPECT_FALSE(AxisAlignedPlane::null.GetBoundingBox());
    EXPECT_THROW(AxisAlignedPlane::null.Project(Coordinate(1,2,3)), std::runtime_error);
    EXPECT_TRUE(std::isnan(AxisAlignedPlane::null.Distance(Coordinate(0,0,0))));
}

TEST(AxisAlignedPlane, XY)
{
    EXPECT_TRUE(AxisAlignedPlane::xy);
    EXPECT_EQ(Plane(AxisAlignedPlane::xy), Plane(Coordinate::origin, Vector::XAxis, Vector::YAxis));
    EXPECT_EQ(0, AxisAlignedPlane::xy.Distance(Coordinate::origin));
    EXPECT_DOUBLE_EQ(1, AxisAlignedPlane::xy.Distance(Coordinate(0, 0, 1)));
    EXPECT_DOUBLE_EQ(0, AxisAlignedPlane::xy.Distance(Coordinate(1, 1, 0)));
    EXPECT_EQ(Coordinate::origin, AxisAlignedPlane::xy.Project(Coordinate(0, 0, 1)));
    EXPECT_EQ(Coordinate(1, 1, 0), AxisAlignedPlane::xy.Project(Coordinate(1, 1, 1)));
    EXPECT_TRUE(AxisAlignedPlane::xy.Above(Coordinate(0, 0, 1)));
    EXPECT_FALSE((-AxisAlignedPlane::xy).Above(Coordinate(0, 0, 1)));
    EXPECT_FALSE(AxisAlignedPlane::xy.Above(Coordinate(0, 0, -1)));
    EXPECT_EQ(AxisAlignedPlane::xy.GetBoundingBox(), AABB(Range<double>::infinity(),
        Range<double>::infinity(),
        Range<double>{0.0,0.0}));
}

TEST(AxisAlignedPlane, XYIntersectsLineSegmentFromFront)
{
    // -7 -6 -5 -4 -3 -2 -1  0  1  2  3
    //     B                 P->   A    Intersects from the front after a partial (2/8) line segments
    EXPECT_EQ(AxisAlignedPlane::xy.Intersection(Line(Coordinate(0,0,2), Coordinate(0, 0, -6))).begin, 0.25);
    EXPECT_FALSE(AxisAlignedPlane::xy.Intersection(Line(Coordinate(0,0,2), Coordinate(0, 0, -6))).IsNumbers());
}

TEST(AxisAlignedPlane, XYIntersectsHalfLineFromBack)
{
    // -7 -6 -5 -4 -3 -2 -1  0  1  2  3
    //     A->B              P->        Intersects from the back after multiple (6) half line vectors
    EXPECT_EQ(AxisAlignedPlane::xy.Intersection(Line(Coordinate(0,0,-6), Vector(0, 0, 1))).end, 6);
    EXPECT_FALSE(AxisAlignedPlane::xy.Intersection(Line(Coordinate(0,0,-6), Vector(0, 0, 1))).IsNumbers());
}

TEST(AxisAlignedPlane, XYDoesntIntersectParallelLine)
{
    // Line parallel to the XY plane (same Z)
    auto i = AxisAlignedPlane::xy.Intersection(Line(Coordinate(0,1,2), Coordinate(1, 0, 2)));
    EXPECT_FALSE(i) << i;
}

TEST(AxisAlignedPlane, XYIntersectsLineBehindOrigin)
{
    // -4 -3 -2 -1  0  1  2
    //     B<-A      P->       Intersects behind A (negative) from behind the plane
    EXPECT_EQ(AxisAlignedPlane::xy.Intersection(Line(Coordinate(0,1,-2), Vector(1, 0, -1))).begin, -2);
}

}
