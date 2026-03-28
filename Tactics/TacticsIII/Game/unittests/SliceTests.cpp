#include "Game/Slice.h"
#include "Game/Block.h"
#include <gtest/gtest.h>

namespace Game::Test
{
using namespace ::testing;

TEST(Slice, EmptySlice)
{
    Slice slice;
    EXPECT_EQ(slice.size(), 0);
    EXPECT_THROW(slice[0], std::out_of_range);
}

TEST(Slice, SolidSlice)
{
    EXPECT_EQ(Slice(Block::Stone)[0].material, Material::stone);
}

TEST(Slice, LiquidSlice)
{
    EXPECT_EQ(Slice(Block::Water)[0].material, Material::water);
}

TEST(Slice, AirSlice)
{
    EXPECT_EQ(Slice(Block::Air)[0].material, Material::air);
}

TEST(Slice, MixedSlice)
{
    Block block(0.5, 0.1, 0.0);
    EXPECT_EQ(Slice(block)[0].material, Material::stone);
    EXPECT_EQ(Slice(block)[1].material, Material::water);
    EXPECT_EQ(Slice(block)[2].material, Material::air);
}

TEST(Slice, StackMixesSlice)
{
    auto slice = Slice(Block::Stone) + Slice(Block::Air);
    EXPECT_EQ(slice.size(), 2);
}

TEST(Slice, StackSameSlice)
{
    auto slice = Slice(Block::Stone) + Slice(Block::Stone);
    EXPECT_EQ(slice.size(), 1);
    EXPECT_EQ(slice[0].amount, 2.0);
}


}
