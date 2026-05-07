#include "Utility/MutableTuple.h"
#include <gtest/gtest.h>

namespace Engine::Test
{

TEST(MutableTuple, Void)
{
    EXPECT_EQ(MutableTuple<>().size(), 0);
    EXPECT_EQ(MutableTuple<>().as_tuple(), std::tuple<>());
    EXPECT_THROW(MutableTuple<>().set<bool>(0, true), std::out_of_range);
    EXPECT_THROW(MutableTuple<>().get<bool>(0), std::out_of_range);
    EXPECT_THROW(MutableTuple<>().acceptsTypeAt<bool>(0), std::out_of_range);
}

TEST(MutableTuple, Single)
{
    MutableTuple<int> mutuple;
    EXPECT_EQ(mutuple.size(), 1);
    EXPECT_TRUE(mutuple.acceptsTypeAt<int>(0));
    mutuple.set<int>(0, 1);
    EXPECT_EQ(mutuple.as_tuple(), std::make_tuple(1));
    EXPECT_EQ(mutuple.get<int>(0), 1);

    mutuple[0] = 2;
    EXPECT_EQ(std::get<int>(mutuple[0]), 2);

    EXPECT_THROW(mutuple.get<int>(1), std::out_of_range);
    EXPECT_THROW(mutuple.acceptsTypeAt<int>(1), std::out_of_range);
}

TEST(MutableTuple, Double)
{
    MutableTuple<bool, int> mutuple;
    EXPECT_EQ(mutuple.size(), 2);
    EXPECT_TRUE(mutuple.acceptsTypeAt<bool>(0));
    mutuple.set<bool>(0, true);
    mutuple[1] = 3;
    EXPECT_EQ(mutuple.as_tuple(), std::make_tuple(true, 3));
    EXPECT_EQ(mutuple.get<bool>(0),true);
    EXPECT_EQ(mutuple.get<int>(1), 3);
    EXPECT_EQ(std::get<int>(mutuple[1]), 3);
}

}
