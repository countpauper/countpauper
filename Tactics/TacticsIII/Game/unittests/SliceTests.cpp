#include "Game/Slice.h"
#include "Game/Block.h"
#include "Geometry/Gtest.h"
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

TEST(Slice, HomogeneousCut)
{
    Engine::Range<double> range(0.2, 0.5);
    auto cut = Slice(Block::Stone) & range;
    ASSERT_EQ(cut.size(), 1);
    EXPECT_NEAR(cut[0].amount, range.Size(), 1e-6f);
}

TEST(Slice, HeteroGeneousCut)
{
    Engine::Range<double> range(0.2, 0.8);
    // Original:  |stone| air    |      0.4 stone 0.6 air
    // Cut           |        |         0.2 stone 0.4 air 
    //            0 .2 .4 .6 .8 1.0
    auto cut = Slice(Block(0.4f)) & range;
    ASSERT_EQ(cut.size(), 2);
    EXPECT_EQ(cut[0], (Slice::Layer{Material::stone, 0.2, 300.0f}));
    EXPECT_EQ(cut[1], (Slice::Layer{Material::air, 0.4, 300.0f}));
}

TEST(Slice, ZeroCut)
{
    Engine::Range<double> range(0.2, 0.2);
    auto cut = Slice(Block::Water) & range;
    ASSERT_EQ(cut.size(), 1);
    EXPECT_EQ(cut[0], (Slice::Layer{Material::water, 0.0, 300.0f}));
}

TEST(Slice, EmptyCut)
{
    auto range = Engine::Range<double>::empty();
    auto cut = Slice(Block::Water) & range;
    ASSERT_EQ(cut.size(), 0);
}

TEST(Slice, Scale)
{
    auto halfSlice = Slice(Block(0.4f)) * 0.5;
    ASSERT_EQ(halfSlice.size(), 2);
    EXPECT_EQ(halfSlice[0], (Slice::Layer{Material::stone, 0.2, 300.0f}));
    EXPECT_EQ(halfSlice[1], (Slice::Layer{Material::air, 0.3, 300.0f}));
}

TEST(Slice, Find)
{
    EXPECT_FALSE(Slice(Block::Stone).FindNonSolidOpening());
    EXPECT_EQ(Slice(Block::Air).FindGasOpening().Size(), 1.0);
    EXPECT_RANGE_NEAR(((Slice(Block(0.6)) + Slice(Block(0.4, 0.3)) + Slice(Block::Stone)) * 0.5).FindNonSolidOpening(), 
        Engine::Range<double>(0.7, 1.0), 1e-6);
}

}
