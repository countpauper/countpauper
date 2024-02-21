#include <gtest/gtest.h>
#include <sstream>

#include "Geometry/Box.h"
#include "Utility/Range.h"

namespace Engine::Test
{

TEST(Box, Size)
{
    EXPECT_EQ(1, Box(Position(0, 0, 0), Position(1, 1, 1)).Volume());
    EXPECT_EQ(Size(1, 1, 1), Box(Position(0, 0, 0), Position(1, 1, 1)).Extent());
    EXPECT_EQ(Range(-1, 0), Box(Position(-1, 1, 3), Position(0, 2, 4)).x);
    EXPECT_EQ(Range(1, 2), Box(Position(-1, 1, 3), Position(0, 2, 4)).y);
    EXPECT_EQ(Range(3, 4), Box(Position(-1, 1, 3), Position(0, 2, 4)).z);
    // no expectations yet for size and extent of flipped bounding boxes
}

TEST(Box, Grow)
{
    Box box(Size(1, 1, 1));

    box.Grow(1);
    EXPECT_EQ(3*3*3, box.Volume()); // 3x3x3
    box.Grow(Size(0, 1, 0));
    EXPECT_EQ(3*5*3, box.Volume()); // 3x5x3
}

TEST(Box, Translate)
{
    Box box(Size(1, 1, 1));
    box += Position(1, 1, 1);
    EXPECT_FALSE(box.Contains(Position(0, 0, 0)));
    EXPECT_EQ(Position(1, 1, 1), box.Start());
}

TEST(Box, Include)
{
    Box box(Position(0, 0, 0), Position(2, 2, 2));

    EXPECT_TRUE((box | Position(2, 2, 2))[Position(1, 1, 1)]);
}

}
