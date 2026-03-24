#include <gtest/gtest.h>
#include "Geometry/Vector.h"
#include "Geometry/AxisAlignedBoundingBox.h"
#include "Geometry/Matrix.h"
#include "Geometry/Angles.h"
#include "GTestGeometry.h"


namespace Engine::Test
{

TEST(AABB, Size)
{
    EXPECT_EQ(1, AABB(Coordinate::origin, Coordinate(1, 1, 1)).Volume());
    EXPECT_EQ(Vector(1, 1, 1), AABB(Coordinate::origin, Coordinate(1, 1, 1)).Extent());
    EXPECT_EQ(Range(-1.0, 0.0), AABB(Coordinate(-1, 1, 3), Coordinate(0, 2, 4)).X());
    EXPECT_EQ(Range(1.0, 2.0), AABB(Coordinate(-1, 1, 3), Coordinate(0, 2, 4)).Y());
    EXPECT_EQ(Range(3.0, 4.0), AABB(Coordinate(-1, 1, 3), Coordinate(0, 2, 4)).Z());
    // no expectations yet for size and extent of flipped bounding boxes
}

TEST(AABB, Expand)
{
    AABB box(Coordinate::origin, Coordinate(1, 1, 1));
    box.Expand(Vector(1, -1, 0));
    EXPECT_EQ(4, box.Volume()); // 2x2x1
    EXPECT_EQ(Range(0.0, 2.0), box.X());
    EXPECT_EQ(Range(-1.0, 1.0), box.Y());

    box.Grow(1);
    EXPECT_EQ(48, box.Volume()); // 4x4x3
    EXPECT_EQ(Range(-1.0, 2.0), box.Z());
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

TEST(AABB, AlignedIntersection)
{
    AABB box(Coordinate{0.5, 0.5, 0.5}, Coordinate{1.0, 1.0, 1.0});
    EXPECT_EQ( box.Intersection(Line(Coordinate{0.0, 0.75, 0.75}, Coordinate{1.25, 0.75, 0.75})),
        Range(0.4, 0.8));
    EXPECT_EQ(box.Intersection(Line(Coordinate{0.75, 0.75, 0.75}, Coordinate{1.25, 0.75, 0.75})),
        Range(-0.5, 0.5));
    EXPECT_EQ(box.Intersection(Line(Coordinate{1.25, 0.75, 0.75}, Coordinate{0.75, 0.75, 0.75})),
        Range(0.5, 1.5));
}

TEST(AABB, AlignedMissingIntersection)
{
    AABB box(Coordinate{0.5, 0.5, 0.5}, Coordinate{1.0, 1.0, 1.0});
    EXPECT_FALSE(box.Intersection(Line(Coordinate{0.0, 0.0, 0.75}, Coordinate{1.0, 0.0, 0.75})));
    EXPECT_FALSE(box.Intersection(Line(Coordinate{0.75, 0.0, 1.2}, Coordinate{0.75, 1.0, 1.2})));
    EXPECT_FALSE(box.Intersection(Line(Coordinate{0.0, 0.75, 0.0}, Coordinate{0.0, 0.75, 1.25})));
}

TEST(AABB, UnalignedIntersection)
{
/*
       =|====
       =|===/    Exit  = (4,6)  after 1 2/3th
       =|==B=    B     = (2.5,3)
       =|=/==    Entry = (1,1) after 1/3th of the line
        |/
       -|----------X-axis ----->
        |A       A     = (0.25, 0)
    -1  0
*/
    AABB box(Coordinate{-1.0, 1.0, 0.5}, Coordinate{4.0, 6.0, 1.0});
    EXPECT_EQ( box.Intersection(Line(Coordinate{0.25, 0.0, 0.75}, Coordinate{2.5, 3.0, 0.75})),
        Range(1.0/3, 5.0/3));

}

TEST(AABB, UnalignedMiss)
{

/*      |   /          Pass at 200% (3,4)
        |  /
        | /B           B = (2,1.5)
     1  |/  ==
       -|---==-------X-axis ----->
        |A  ==        A = (0.0, 0)
    -1  0
*/

    AABB box(Coordinate{ 1.0,-1.0, 0.5}, Coordinate{3.0, 1.0, 1.0});
    auto miss = box.Intersection(Line(Coordinate{0.0, 0.0, 0.75}, Coordinate{1.5, 2.0, 0.75}));
    EXPECT_FALSE( miss ) << miss;
}

TEST(AABB, NamedAlignedEntry)
{
    AABB box(Coordinate{0.5, 0.5, 0.5}, Coordinate{1.0, 1.0, 1.0});
    EXPECT_EQ( box.Entry(Line(Coordinate{0.0, 0.75, 0.75}, Coordinate{1.25, 0.75, 0.75})),
        std::make_pair(Orientation::left, 0.4));
    EXPECT_EQ( box.Entry(Line(Coordinate{1.25, 0.75, 0.75}, Coordinate{0.75, 0.75, 0.75})),
        std::make_pair(Orientation::right, 0.5));
    EXPECT_FALSE( box.Entry(Line(Coordinate{0.0, 0.0, 0.75}, Coordinate{1.25, 0.0, 0.75})).first);
    EXPECT_EQ( box.Entry(Line(Coordinate{0.75, 0.75, 0.75}, Coordinate{0.75, 0.25, 0.75})),
        std::make_pair(Orientation::front, -0.5));  // line start in box, this is where it entered if it's infinite.
}


TEST(AABB, NamedAlignedExit)
{
    AABB box(Coordinate{0.5, 0.5, 0.5}, Coordinate{1.0, 1.0, 1.0});
    EXPECT_EQ( box.Exit(Line(Coordinate{0.0, 0.75, 0.75}, Coordinate{1.25, 0.75, 0.75})),
        std::make_pair(Orientation::right, 0.8));
    EXPECT_EQ( box.Exit(Line(Coordinate{1.25, 0.75, 0.75}, Coordinate{0.75, 0.75, 0.75})),
        std::make_pair(Orientation::left, 1.5));
    EXPECT_FALSE( box.Exit(Line(Coordinate{0.0, 0.0, 0.75}, Coordinate{1.25, 0.0, 0.75})).first);
    EXPECT_EQ( box.Exit(Line(Coordinate{0.75, 1.25, 0.75}, Coordinate{0.75, 1.75, 0.75})),
        std::make_pair(Orientation::front, -0.5));  // line start on box, this is where it would exit if it's infinite.
}

TEST(AABB, NamedUnalignedEntry)
{
    AABB box(Coordinate{0.5, 0.5, 0.5}, Coordinate{1.0, 1.0, 1.0});
    auto diagonal = Line(Coordinate{0.0, 0.25, 0.25}, Coordinate{1.25, 1.0, 0.75});
    auto diagonalHit = box.Entry(diagonal);
    EXPECT_EQ(diagonalHit.first, Orientation::down);
    auto diagonalHitLocation = diagonal.Section(Range<double>(diagonalHit.second, 10.0)).A();
    EXPECT_DOUBLE_EQ(diagonalHitLocation.Z(), box.z.begin);
    EXPECT_TRUE(box.X()[diagonalHitLocation.X()]);
    EXPECT_TRUE(box.Y()[diagonalHitLocation.Y()]);
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

