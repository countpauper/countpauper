#include <gtest/gtest.h>
#include <sstream>

#include "Geometry/IntBox.h"
#include "Utility/Range.h"

namespace Engine::Test
{

TEST(IntBox, Size)
{
    EXPECT_EQ(IntBox(Position(1, 1, 1)).Volume(), 1);
    EXPECT_EQ(IntBox(Position(0, 0, 0), Position(1, 1, 1)).Volume(), 1);
    EXPECT_EQ(IntBox(Position(0, 0, 0), Position(1, 1, 1)).Extent(), Size(1, 1, 1));
    EXPECT_EQ(IntBox(Position(-1, 1, 3), Position(0, 2, 4)).x, Range(-1, 0));
    EXPECT_EQ(IntBox(Position(-1, 1, 3), Position(0, 2, 4)).y, Range(1, 2));
    EXPECT_EQ(IntBox(Position(-1, 1, 3), Position(0, 2, 4)).z, Range(3, 4));
    // no expectations yet for size and extent of flipped bounding boxes
}

TEST(IntBox, Grow)
{
    IntBox box(Size(1, 1, 1));

    box.Grow(1);
    EXPECT_EQ(box.Volume(), 3*3*3); // 3x3x3
    box.Grow(Size(0, 1, 0));
    EXPECT_EQ(box.Volume(), 3*5*3); // 3x5x3
}

TEST(IntBox, Translate)
{
    IntBox box(Size(1, 1, 1));
    box += Position(1, 1, 1);
    EXPECT_FALSE(box.Contains(Position(0, 0, 0)));
    EXPECT_EQ(box.Start(), Position(1, 1, 1));
}

TEST(IntBox, Include)
{
    IntBox box(Position(0, 0, 0), Position(2, 2, 2));

    EXPECT_TRUE((box | Position(2, 2, 2))[Position(1, 1, 1)]);
}


TEST(IntBox, Iterator)
{
    IntBox box(Position(-1, 0, -1), Position(2,1,1));
    std::vector<Position> positions;
    std::copy(box.begin(), box.end(), std::back_inserter(positions));
    EXPECT_EQ(positions, std::vector<Position>({
        {-1, 0, -1},
        {0, 0, -1},
        {1, 0, -1},

        {-1, 0, 0},
        {0, 0, 0},
        {1, 0, 0},

    }));
}
}
