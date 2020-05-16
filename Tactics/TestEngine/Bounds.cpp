#include "pch.h"
#include "Range.h"
#include "Vector.h"
#include "AxisAlignedBoundingBox.h"


namespace Engine::Test
{
TEST(Range, Size)
{
    EXPECT_EQ(1, Range(0, 1).Size());
    EXPECT_EQ(0, Range(2, 2).Size());
    EXPECT_FALSE(Range(3, 3));
}

TEST(Range, Union)
{
    EXPECT_EQ(Range(0, 2), Range(0, 1) | Range(1, 2));
    EXPECT_EQ(Range(-1, 2), Range(-1, 0) | Range(1, 2));
    EXPECT_EQ(Range(-1, 2), Range(1, 2) | Range(-1, 0));
}

TEST(Range, Intersection)
{
    EXPECT_FALSE(Range(0, 1) & Range(1, 2));
    EXPECT_EQ(Range(0, 1), Range(-1, 2) & Range(0, 1));
    EXPECT_EQ(Range(0, 1), Range(-1, 1) & Range(0, 3));
    EXPECT_FALSE(Range(2, 3) & Range(0, 1));
}

TEST(Range, Expand)
{
    EXPECT_EQ(2, (Range(0, 1) + 1).Size());
}

TEST(Range, Inclusion)
{
    EXPECT_TRUE((Range<int>() | 2)[2]);
    EXPECT_TRUE((Range<int>() | -1)[-1]);
    EXPECT_TRUE((Range<int>() | -1 | 2)[1]);
    EXPECT_FALSE((Range<int>() | -1)[2]);
}

TEST(AABB, Size)
{
    EXPECT_EQ(1, AABB(Coordinate(0, 0, 0), Coordinate(1, 1, 1)).Volume());
    EXPECT_EQ(Vector(1, 1, 1), AABB(Coordinate(0, 0, 0), Coordinate(1, 1, 1)).Extent());
    EXPECT_EQ(Range(-1, 0), AABB(Coordinate(-1, 1, 3), Coordinate(0, 2, 4)).X());
    EXPECT_EQ(Range(1, 2), AABB(Coordinate(-1, 1, 3), Coordinate(0, 2, 4)).Y());
    EXPECT_EQ(Range(3, 4), AABB(Coordinate(-1, 1, 3), Coordinate(0, 2, 4)).Z());
    // no expectations yet for size and extent of flipped bounding boxes 
}

TEST(AABB, Expand)
{
    AABB box(Coordinate(0, 0, 0), Coordinate(1, 1, 1));
    box.Expand(Vector(1, -1, 0));
    EXPECT_EQ(4, box.Volume()); // 2x2x1
    EXPECT_EQ(Range(0, 2), box.X());
    EXPECT_EQ(Range(-1, 1), box.Y());

    box.Expand(1);
    EXPECT_EQ(48, box.Volume()); // 4x4x3
    EXPECT_EQ(Range(-1, 2), box.Z());
}

TEST(AABB, Include)
{
    AABB box(Coordinate(0, 0, 0), Coordinate(1, 1, 1));
    
    EXPECT_TRUE((box | Coordinate(2, 2, 2))[Coordinate(1.5, 1.5, 1.5)]);
}

}