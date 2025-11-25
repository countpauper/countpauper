#include "Game/Comparator.h"
#include <gtest/gtest.h>

namespace Game::Test
{

TEST(Comparator, equality)
{
    EXPECT_EQ(Comparator(Comparator::not_equal), Comparator(Comparator::not_equal));
    EXPECT_NE(Comparator(Comparator::equal), Comparator(Comparator::not_equal));
    EXPECT_EQ(Comparator(Comparator::greater), Comparator(Comparator::greater));
    EXPECT_NE(Comparator(Comparator::greater), Comparator(Comparator::greater_equal));
}

TEST(Comparator, to_string)
{
    EXPECT_EQ(std::string(Comparator(Comparator::equal)), "is");
    EXPECT_EQ(std::string(Comparator(Comparator::not_equal)), "is not");
    EXPECT_EQ(std::string(Comparator(Comparator::greater)), "is more than");
    EXPECT_EQ(std::string(Comparator(Comparator::greater_equal)), "is not less than");
}

TEST(Comparator, opposites)
{
    EXPECT_EQ(Comparator(Comparator::equal).Opposite(), Comparator(Comparator::not_equal));
    EXPECT_EQ(Comparator(Comparator::not_equal).Opposite(), Comparator(Comparator::equal));
    EXPECT_EQ(Comparator(Comparator::greater).Opposite(), Comparator(Comparator::less_equal));
    EXPECT_EQ(Comparator(Comparator::greater_equal).Opposite(), Comparator(Comparator::less));
}

TEST(Comparator, compare)
{
    EXPECT_TRUE(Comparator(Comparator::equal)(3,3));
    EXPECT_FALSE(Comparator(Comparator::equal)(3,-1));
    EXPECT_TRUE(Comparator(Comparator::not_equal)(3,-1));
    EXPECT_TRUE(Comparator(Comparator::greater)(1, -3));
    EXPECT_TRUE(Comparator(Comparator::less_equal)(-3,1));
    EXPECT_TRUE(Comparator(Comparator::less_equal)(1,1));
    EXPECT_FALSE(Comparator(Comparator::less_equal)(1,-1));
}

}
