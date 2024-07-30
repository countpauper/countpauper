#include <gtest/gtest.h>
#include "Game/Computation.h"

namespace Game::Test
{
using namespace ::testing;

TEST(Computation, empty)
{
    Computation c;
    EXPECT_TRUE(c.empty());
    EXPECT_EQ(c.Description(), "");
    EXPECT_EQ(c.Total(), 0);
}

TEST(Computation, constant)
{
    Computation c("test", 3);
    EXPECT_FALSE(c.empty());
    EXPECT_EQ(c.Description(), "3[test]");
    EXPECT_EQ(c.Total(), 3);
}

TEST(Computation, add)
{
    Computation a("a", 3);
    Computation b("b", 2);
    auto c = a + b;

    EXPECT_FALSE(c.empty());
    EXPECT_EQ(c.Description(), "3[a] + 2[b]");
    EXPECT_EQ(c.Total(), 5);
}

TEST(Computation, subtract)
{
    Computation a("a", 3);
    Computation b("b", 2);
    auto c = a - b;

    EXPECT_FALSE(c.empty());
    EXPECT_EQ(c.Description(), "3[a] - 2[b]");
    EXPECT_EQ(c.Total(), 1);
}

}
