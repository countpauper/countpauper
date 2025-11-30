#include <gtest/gtest.h>
#include "Game/Position.h"

namespace Game::Test
{

TEST(Position, Bool)
{
    EXPECT_FALSE(Position());
    EXPECT_TRUE((Position{0, 0, 0.1f}));
}

TEST(Position, Size)
{
    EXPECT_EQ(0, Position().Size());
    EXPECT_EQ(1.0, Position(1, 0, 0).Size());
    EXPECT_EQ(1.0, Position(0, 0, -1.0f).Size());
}

TEST(Position, ManhattanDistance)
{
    EXPECT_EQ(0.0f, Position(1, 1, 0).ManDistance(Position(1, 1, 0)));
    EXPECT_FLOAT_EQ(1.0f, Position(0, 1, 0.5f).ManDistance(Position(1, 1, 0.5f)));
    EXPECT_FLOAT_EQ(2.1f, Position(0, 1, 0.1f).ManDistance(Position(1, 0, 0.2f)));
    EXPECT_FLOAT_EQ(3.0f, Position(0, 1, 0).ManDistance(Position(1, -1, 0)));
    EXPECT_FLOAT_EQ(1.6f, Position(0, 1, 0.1f).ManDistance(Position(0, 0, 0.7f)));
    EXPECT_FLOAT_EQ(2.9f, Position(0, 1, 0).ManDistance(Position(0, -1, -0.9f)));
}

}

