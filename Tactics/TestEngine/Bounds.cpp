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

TEST(Range, Middle)
{
    EXPECT_EQ(3, Range<int>(2, 4).Middle());
    EXPECT_EQ(2, Range<int>(2, 3).Middle());
    EXPECT_EQ(1.5, Range<double>(1, 2).Middle());
    EXPECT_EQ(1.5, Range<double>(2, 1).Middle());
}

TEST(Range, MiddleOfEverything)
{
    EXPECT_EQ(0, Range<double>::infinity().Middle());
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

TEST(Range, Move)
{
    EXPECT_EQ(1, (Range(0, 1) + 1).Size());
}

TEST(Range, Expand)
{
    EXPECT_EQ(2, Range(0, 1).Expand(1).Size());
}


TEST(Range, consts)
{
    EXPECT_EQ(std::numeric_limits<double>::infinity(), Range<double>::infinity().Size());
    EXPECT_TRUE(Range<double>::infinity() | -10000.0);
    EXPECT_EQ(0, Range<double>::null().Size());
    EXPECT_FALSE(Range<double>::null() | 0);
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

    box.Grow(1);
    EXPECT_EQ(48, box.Volume()); // 4x4x3
    EXPECT_EQ(Range(-1, 2), box.Z());
}

TEST(AABB, Include)
{
    AABB box(Coordinate(0, 0, 0), Coordinate(1, 1, 1));
    
    EXPECT_TRUE((box | Coordinate(2, 2, 2))[Coordinate(1.5, 1.5, 1.5)]);
}


TEST(AABB, Clip)
{
    AABB box(Coordinate(0, 0, 0), Coordinate(1, 1, 1));

    EXPECT_TRUE(box.Contains(box.Clip(Coordinate(2, -1, 0))));
    EXPECT_TRUE(box.Contains(box.Clip(Coordinate(-std::numeric_limits<double>::infinity(), 0.5, 10.0))));
}

}