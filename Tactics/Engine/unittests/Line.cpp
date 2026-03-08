#include <gtest/gtest.h>
#include "Geometry/Line.h"
#include <numeric>

namespace Engine::Test
{

TEST(Line, Length)
{
    Coordinate a(1, 2, 3);
    Coordinate b(3, 0, 4);
    EXPECT_EQ(3.0, Line( a, b ).Length());
}

TEST(Line, Distance)
{
    Line l0(Coordinate(1, 0, 0), Coordinate(1, 0, 0));
    EXPECT_EQ(1.0, l0.Distance(Coordinate(1, -1, 0)));

    Line l1(Coordinate(1, 0, 0), Coordinate(3, 2, 0));
    EXPECT_EQ(l1.Distance(Coordinate(1, -1, 0)), 1.0);
    EXPECT_EQ(l1.Distance(Coordinate(5, 2, 0)), 2.0);
    EXPECT_EQ(l1.Distance(Coordinate(0, 3, 1)), 3.0);

    Line l2(Coordinate(1, 1, 0), Coordinate(1, 1, 0));
    EXPECT_EQ(1.0, l2.Distance(Coordinate(1, 0, 0)));
}



TEST(Line, Section)
{
    Coordinate a(1, 2, 3);
    Coordinate b(3, 0, 4);
    Line line(a, b);
    auto firstSection = line.Section(Range(0.0, line.Length() / 2.0));
    EXPECT_EQ(line.a, firstSection.a);
    EXPECT_EQ(firstSection.b, Coordinate(2, 1, 3.5));
    auto secondSection = line.Section(Range(line.Length() / 2.0, line.Length()));
    EXPECT_EQ(line.b, secondSection.b);
    EXPECT_EQ(secondSection.a, firstSection.b);
    auto clippedSection = line.Section(Range(0.0, std::numeric_limits<double>::infinity()));
    EXPECT_EQ(clippedSection, line);

}

TEST(Line, Vozelize)
{
    Coordinate a(0.5, 2.2, 3.6);
    Coordinate b(3.1, -0.5, -2.0);

    //     Y                 Za
    //     3                 3*
    //     2a*               2 *
    //     1  **             1  *
    //X -1 0 1 2*3        X 0 1 2*3
    //    -1     *b         -1    *
    //                      -2     b

    Line line(a,b);
    auto voxels = line.Voxelize();
    EXPECT_LE(voxels.size(), 3+3+6);
    EXPECT_EQ(voxels[0].first, Position(0, 2, 3));
    EXPECT_EQ(voxels[1].first, Position(0, 1, 3));
    EXPECT_EQ(voxels.back().first, Position(3,-1,-2));
    auto total_fragments = std::accumulate(voxels.begin(), voxels.end(), 0.0,
        [](double sum, std::pair<Position, double> voxel)
    {
        return sum + voxel.second;
    });
    EXPECT_DOUBLE_EQ(total_fragments, line.Length());

}


}
