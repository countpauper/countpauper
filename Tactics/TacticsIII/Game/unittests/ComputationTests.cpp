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

TEST(Computation, value)
{
    Computation c(3, "test");
    EXPECT_FALSE(c.empty());
    EXPECT_FALSE(c.IsConstant());
    EXPECT_EQ(c.Description(), "3[test]");
    EXPECT_EQ(c.Total(), 3);
}

TEST(Computation, constatn)
{
    Computation c(3);
    EXPECT_FALSE(c.empty());
    EXPECT_TRUE(c.IsConstant());
    EXPECT_EQ(c.Description(), "3");
    EXPECT_EQ(c.Total(), 3);
}

TEST(Computation, add)
{
    Computation a(3, "a");
    Computation b(2, "b");
    auto c = a + b;

    EXPECT_FALSE(c.empty());
    EXPECT_EQ(c.Description(), "3[a] + 2[b]");
    EXPECT_EQ(c.Total(), 5);
}

TEST(Computation, subtract)
{
    Computation a(3, "a");
    Computation b(2, "b");
    auto c = a - b;

    EXPECT_FALSE(c.empty());
    EXPECT_EQ(c.Description(), "3[a] - 2[b]");
    EXPECT_EQ(c.Total(), 1);
}

TEST(Computation, multiply)
{
    Computation a(3, "a");
    Computation b(2, "b");
    auto c = a * b;

    EXPECT_FALSE(c.empty());
    EXPECT_EQ(c.Description(), "3[a] * 2[b]");
    EXPECT_EQ(c.Total(), 6);
}

TEST(Computation, nested_multiply)
{
    Computation a(3, "a");
    Computation b = Computation(2, "b") - Computation(1, "c");
    auto c = a * b;

    EXPECT_FALSE(c.empty());
    EXPECT_EQ(c.Description(), "3[a] * 1[2[b] - 1[c]]");
    EXPECT_EQ(c.Total(), 3);
}

TEST(Computation, limit)
{   // TODO: this limit is a remnamnt  from stat specific descriptors, which were computations
    // preferably upper and lower bound operators are used if there's a mechanism to keep these add the end as operands are appended
    Computation limited(Engine::Range<int>(0,5));
    limited += Computation(7, "a");
    EXPECT_EQ(limited.Total(), 4);  // NB range is exclusive on the maximum
    limited -= Computation(10, "b");
    EXPECT_EQ(limited.Total(), 0);
}


TEST(Computation, lower_bound)
{
    auto c = Computation(-1, "a") >= Computation(0, "limit");


    EXPECT_FALSE(c.empty());
    EXPECT_EQ(c.Description(), "0[limit]");
    EXPECT_EQ(c.Total(), 0);
}

TEST(Computation, not_lower_bound)
{
    auto c = Computation(3, "a") >= Computation(1, "limit");

    EXPECT_FALSE(c.empty());
    EXPECT_EQ(c.Description(), "3[a]");
    EXPECT_EQ(c.Total(), 3);
}

}
