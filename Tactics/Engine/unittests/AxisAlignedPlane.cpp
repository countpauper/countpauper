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
}

TEST(AxisAlignedPlane, XY)
{
    EXPECT_TRUE(AxisAlignedPlane::xy);
    EXPECT_EQ(Plane(AxisAlignedPlane::xy), Plane(Coordinate::origin, Vector::X, Vector::Y));
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

TEST(AxisAlignedPlane, XYIntersection)
{
    EXPECT_EQ(AxisAlignedPlane::xy.Intersection(Line(Coordinate(0,0,2), Coordinate(0, 0, -6))).begin, 2);
    EXPECT_EQ(AxisAlignedPlane::xy.Intersection(Line(Coordinate(0,0,-6), Coordinate(0, 0, 2))).begin, -6);
    EXPECT_FALSE(AxisAlignedPlane::xy.Intersection(Line(Coordinate(0,1,2), Coordinate(1, 0, 2))));
    EXPECT_FALSE(AxisAlignedPlane::xy.Intersection(Line(Coordinate(0,0,2), Coordinate(0, 0, 1))));
    EXPECT_FALSE(AxisAlignedPlane::xy.Intersection(Line(Coordinate(0,0,-2), Coordinate(0, 0, -4))));
}

}
