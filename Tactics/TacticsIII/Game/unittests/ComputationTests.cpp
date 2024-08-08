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

TEST(Computation, multiply)
{
    Computation a("a", 3);
    Computation b("b", 2);
    auto c = a * b;

    EXPECT_FALSE(c.empty());
    EXPECT_EQ(c.Description(), "3[a] * 2[b]");
    EXPECT_EQ(c.Total(), 6);
}

TEST(Computation, nested_multiply)
{
    Computation a("a", 3);
    Computation b = Computation("b", 2) - Computation("c", 1);
    auto c = a * b;

    EXPECT_FALSE(c.empty());
    EXPECT_EQ(c.Description(), "3[a] * 1[2[b] - 1[c]]");
    EXPECT_EQ(c.Total(), 3);
}

TEST(Computation, lower_bound)
{
    auto c = Computation("a", -1) >= Computation("limit", 0);


    EXPECT_FALSE(c.empty());
    EXPECT_EQ(c.Description(), "0[limit]");
    EXPECT_EQ(c.Total(), 0);
}

TEST(Computation, not_lower_bound)
{
    auto c = Computation("a", 3) >= Computation("limit", 1);

    EXPECT_FALSE(c.empty());
    EXPECT_EQ(c.Description(), "3[a]");
    EXPECT_EQ(c.Total(), 3);
}

}
