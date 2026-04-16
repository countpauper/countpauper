#include <gtest/gtest.h>
#include "Game/Position.h"

namespace Game::Test
{

TEST(Position, Bool)
{
    EXPECT_FALSE(Position());
    EXPECT_TRUE((Position{0, 0, 0.1f}));
}

TEST(Position, Length)
{
    EXPECT_EQ(Position().Length(), 0);
    EXPECT_EQ(Position(1, 0, 0).Length(), 1.0);
    EXPECT_EQ(Position(0, 0, -1.0f).Length(), 1.0);
}

TEST(Position, Delta)
{
    EXPECT_EQ(Position(1, 0, 0) + Position(0, -2, 0.1), Position(1, -2, 0.1));
    EXPECT_EQ(Position(1, 0, 0) - Position(2, -2, 0.1), Position(-1, 2, -0.1));
}

TEST(Position, ManhattanDistance)
{
    EXPECT_EQ( Position(1, 1, 0).ManDistance(Position(1, 1, 0)), 0.0f);
    EXPECT_FLOAT_EQ(Position(0, 1, 0.5f).ManDistance(Position(1, 1, 0.5f)), 1.0f);
    EXPECT_FLOAT_EQ(Position(0, 1, 0.1f).ManDistance(Position(1, 0, 0.35f)), 2.25f);
    EXPECT_FLOAT_EQ(Position(0, 1, 0).ManDistance(Position(1, -1, 0)), 3.0f);
    EXPECT_FLOAT_EQ(Position(0, 1, 0.1f).ManDistance(Position(0, 0, 0.6f)), 1.5f);
    EXPECT_FLOAT_EQ(Position(0, 1, 0).ManDistance(Position(0, -1, -0.75f)), 2.75f);
}

TEST(Position, Distance)
{
    EXPECT_EQ( Position(1, 1, 0).Distance(Position(1, 1, 0)), 0.0f);
    EXPECT_FLOAT_EQ(Position(0, 1, 0.5f).Distance(Position(1, 1, 0.5f)), 1.0f);
    EXPECT_FLOAT_EQ(Position(-1, 0, 0.1f).Distance(Position(0, 0, 1.1f)), std::sqrt(2));
}
}

