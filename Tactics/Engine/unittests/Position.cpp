#include <gtest/gtest.h>
#include "Geometry/Position.h"

namespace Engine::Test
{

TEST(Position, Bool)
{
    EXPECT_FALSE(Position());
    EXPECT_TRUE((Position{1, 0, 0}));
}

TEST(Position, Size)
{
    EXPECT_EQ(0, Position().Size());
    EXPECT_EQ(1.0, Position(1, 0, 0).Size());
    EXPECT_EQ(1.0, Position(0, -1, 0).Size());
}

TEST(Position, ManhattanDistance)
{
    EXPECT_EQ(0, Position(1, 1, 0).ManDistance(Position(1, 1, 0)));
    EXPECT_EQ(1, Position(0, 1, 0).ManDistance(Position(1, 1, 0)));
    EXPECT_EQ(2, Position(0, 1, 0).ManDistance(Position(1, 0, 0)));
    EXPECT_EQ(3, Position(0, 1, 0).ManDistance(Position(1, -1, 0)));
    EXPECT_EQ(2, Position(0, 1, 0).ManDistance(Position(0, 0, 1)));
    EXPECT_EQ(3, Position(0, 1, 0).ManDistance(Position(0, -1, -1)));
}

}
