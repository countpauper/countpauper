#include <gtest/gtest.h>
#include "Utility/Range.h"
#include "Geometry/Vector.h"
#include "Geometry/AxisAlignedBoundingBox.h"
#include "Geometry/Matrix.h"
#include "Geometry/Angles.h"
#include "GTestGeometry.h"


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

TEST(Range, Cast)
{
    EXPECT_EQ(Range<int>(0, 1), Range<int>(Range<double>(0.1, 1.2)));
    EXPECT_EQ(Range<double>(-1.0, 2.0), Range<double>(Range<int>(-1, 2)));
}

TEST(Range, consts)
{
    EXPECT_EQ(std::numeric_limits<double>::infinity(), Range<double>::infinity().Size());
    EXPECT_TRUE(Range<double>::infinity() | -10000.0);
    EXPECT_EQ(0, Range<double>::null().Size());
    EXPECT_FALSE(Range<double>::null() | 0);
    EXPECT_FALSE(Range<double>::empty());
    EXPECT_FALSE(Range<int>::empty());
}

TEST(Range, Inclusion)
{
    EXPECT_TRUE((Range<int>() | 2)[2]);
    EXPECT_TRUE((Range<int>() | -1)[-1]);
    EXPECT_TRUE((Range<int>() | -1 | 2)[1]);
    EXPECT_FALSE((Range<int>() | -1)[2]);
}

TEST(Range, Compare)
{
    EXPECT_TRUE(Range<int>(2,3) > 1 );
    EXPECT_FALSE(Range<int>(2,3) > 2 );
    EXPECT_TRUE(Range<int>(2,4) >= 3 );

    EXPECT_TRUE(Range<int>(2,3) < 4 );
    EXPECT_TRUE(Range<int>(2,3) < 3 );
    EXPECT_FALSE(Range<int>(2,3) < 2 );
    EXPECT_TRUE(Range<int>(2,4) <= 3 );
}

TEST(AABB, Size)
{
    EXPECT_EQ(1, AABB(Coordinate::origin, Coordinate(1, 1, 1)).Volume());
    EXPECT_EQ(Vector(1, 1, 1), AABB(Coordinate::origin, Coordinate(1, 1, 1)).Extent());
    EXPECT_EQ(Range(-1, 0), AABB(Coordinate(-1, 1, 3), Coordinate(0, 2, 4)).X());
    EXPECT_EQ(Range(1, 2), AABB(Coordinate(-1, 1, 3), Coordinate(0, 2, 4)).Y());
    EXPECT_EQ(Range(3, 4), AABB(Coordinate(-1, 1, 3), Coordinate(0, 2, 4)).Z());
    // no expectations yet for size and extent of flipped bounding boxes
}

TEST(AABB, Expand)
{
    AABB box(Coordinate::origin, Coordinate(1, 1, 1));
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
    AABB box(Coordinate::origin, Coordinate(1, 1, 1));

    EXPECT_TRUE((box | Coordinate(2, 2, 2))[Coordinate(1.5, 1.5, 1.5)]);
}


TEST(AABB, Clip)
{
    AABB box(Coordinate::origin, Coordinate(1, 1, 1));

    EXPECT_TRUE(box.Contains(box.Clip(Coordinate(2, -1, 0))));
    EXPECT_TRUE(box.Contains(box.Clip(Coordinate(-std::numeric_limits<double>::infinity(), 0.5, 10.0))));
}

TEST(AABB, Intersection)
{
    AABB box(Coordinate{0.5, 0.5, 0.5}, Coordinate{1.0, 1.0, 1.0});
    EXPECT_EQ( box.Intersection(Line(Coordinate{0.0, 0.75, 0.75}, Coordinate{1.25, 0.75, 0.75})),
        Range(0.5, 1.0));
    EXPECT_EQ(box.Intersection(Line(Coordinate{0.75, 0.75, 0.75}, Coordinate{1.25, 0.75, 0.75})),
        Range(0.0, 0.25));
    EXPECT_EQ(box.Intersection(Line(Coordinate{1.25, 0.75, 0.75}, Coordinate{0.75, 0.75, 0.75})),
        Range(0.25, 0.5));
    EXPECT_FALSE(box.Intersection(Line(Coordinate{0.0, 0.0, 0.75}, Coordinate{1.0, 0.0, 0.75})));
}


TEST(AABB, NamedIntersection)
{
    AABB box(Coordinate{0.5, 0.5, 0.5}, Coordinate{1.0, 1.0, 1.0});
    EXPECT_EQ( box.NamedIntersection(Line(Coordinate{0.0, 0.75, 0.75}, Coordinate{1.25, 0.75, 0.75})),
        std::make_pair(Axis::NegX, 0.5));
    EXPECT_EQ( box.NamedIntersection(Line(Coordinate{1.25, 0.75, 0.75}, Coordinate{0.75, 0.75, 0.75})),
        std::make_pair(Axis::PosX, 0.25));
    EXPECT_EQ( box.NamedIntersection(Line(Coordinate{0.0, 0.0, 0.75}, Coordinate{1.25, 0.0, 0.75})),
        std::make_pair(Axis::None, std::numeric_limits<double>::infinity()));
    EXPECT_EQ( box.NamedIntersection(Line(Coordinate{0.75, 0.75, 0.75}, Coordinate{0.75, 0.00, 0.75})),
        std::make_pair(Axis::NegY, -0.25));

    auto diagonal = Line(Coordinate{0.0, 0.25, 0.25}, Coordinate{1.25, 1.0, 0.75});
    auto diagonalHit = box.NamedIntersection(diagonal);
    EXPECT_EQ(diagonalHit.first, Axis::NegZ);
    auto diagonalHitLocation = diagonal.Section(Range<double>(diagonalHit.second, 10.0)).a;
    EXPECT_DOUBLE_EQ(diagonalHitLocation.z, box.z.begin);
    EXPECT_TRUE(box.X()[diagonalHitLocation.x]);
    EXPECT_TRUE(box.Y()[diagonalHitLocation.y]);
}

TEST(AABB, Transform)
{
    AABB box(Coordinate::origin, Coordinate(1, 1, 1));

    EXPECT_EQ(Vector(2,1,1), (Matrix::Scale(Vector(2, 1, 1)) * box).Extent()); // 0,0,0 ... 2,1,1
    EXPECT_EQ(Vector(1, 1, 1),(Matrix::XRot(PI * 0.5) * box).Extent()); // 0,-1,0 ... 1,0,1
    EXPECT_3D_NEAR(Vector(sqrt(2), 1, sqrt(2)), (Matrix::YRot(PI * 0.25) * box).Extent(), 1e-12); // 0, 0, -sqrt(2)/2 ....sqrt(2),1, sqrt(2)/2  = sqrt(2)*sqrt(2) * 1
    EXPECT_EQ(Coordinate(100.5, 200.5, 300.5),(Matrix::Translation(Vector(100, 200, 300))* box).Center());
    EXPECT_EQ(Vector(1, 1, 1), (Matrix::Translation(Vector(1, 2, 3)) * box).Extent());
}

}

