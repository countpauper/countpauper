#include "Game/Stack.h"
#include "Geometry/Gtest.h"
#include <gtest/gtest.h>



namespace Game::Test
{
using namespace ::testing;

TEST(Stack, EmptySlice)
{
    Stack stack;
    EXPECT_EQ(stack.size(), 0);
    EXPECT_THROW(stack[0], std::out_of_range);
}

TEST(Stack, CreateSlice)
{
    EXPECT_EQ(Stack(Material::stone, 1.0, 0.0)[0].material, Material::stone);
}


TEST(Stack, MixedSlice)
{
    Stack stack{
            {Material::stone, 0.5, 0.0f}, 
            {Material::water, 0.1, 0.0f},
            {Material::air,  0.4, 0.0f}};
    EXPECT_EQ(stack[0].material, Material::stone);
    EXPECT_EQ(stack[1].material, Material::water);
    EXPECT_EQ(stack[2].material, Material::air);
}

TEST(Stack, StackMixesSlice)
{
    auto stack = Stack(Material::stone) + Stack(Material::air);
    EXPECT_EQ(stack.size(), 2);
}

TEST(Stack, StackSameSlice)
{
    auto stack = Stack(Material::stone) + Stack(Material::stone);
    EXPECT_EQ(stack.size(), 1);
    EXPECT_EQ(stack[0].height, 2.0);
}

TEST(Stack, HomogeneousCut)
{
    Engine::Range<ZType> range(0.2f, 0.5f);
    auto cut = Stack(Material::stone) & range;
    ASSERT_EQ(cut.size(), 1);
    EXPECT_EQ(cut[0].height, range.Size());
}

TEST(Stack, HeteroGeneousCut)
{
    Engine::Range<ZType> range(0.2f, 0.9f);
    // Original:  |stone| air    |      0.4 stone 0.6 air
    // Cut           |          |       0.2 stone 0.5 air 
    //            0 .2 .4 .6 .8 1.0
    auto cut = Stack{{Material::stone, 0.4, 300.0}, {Material::air, 0.6, 300.0}} & range;
    ASSERT_EQ(cut.size(), 2);
    EXPECT_EQ(cut[0], (Cell{Material::stone, 0.2, 300.0f}));
    EXPECT_EQ(cut[1], (Cell{Material::air, 0.5, 300.0f, static_cast<Cell::Pressure>(cut[1].GetPressure(0.0, earthGravity))}));
}

TEST(Stack, ZeroCut)
{
    Engine::Range<ZType> range(0.2f, 0.2f);
    auto cut = Stack(Material::water) & range;
    ASSERT_EQ(cut.size(), 1);
    EXPECT_EQ(cut[0], (Cell{Material::water, 0.0, 300.0f, static_cast<Cell::Pressure>(cut[0].GetPressure(0.0, earthGravity))}));
}

TEST(Stack, EmptyCut)
{
    auto range = Engine::Range<ZType>::empty();
    auto cut = Stack(Material::water) & range;
    ASSERT_EQ(cut.size(), 0);
}


TEST(Stack, OverSizedCut)
{
    Engine::Range<ZType> range(-0.5, 1.75);
    auto cut = Stack({{Material::air, 1.0, 0.0f}}) & range;
    ASSERT_EQ(cut.size(), 3);
    EXPECT_EQ(cut[0], (Cell{Material::vacuum, 0.5, 0.0f}));
    EXPECT_EQ(cut[1], (Cell{Material::air, 1.0, 0.0f}));
    EXPECT_EQ(cut[2], (Cell{Material::vacuum, 0.75, 0.0f}));
}

TEST(Stack, Scale)
{
    auto halfSlice = Stack{{Material::stone, 0.4, 300.0}, {Material::air, 0.6, 300.0}} * 0.5f;
    ASSERT_EQ(halfSlice.size(), 2);
    EXPECT_EQ(halfSlice[0], (Cell{Material::stone, 0.2, 300.0f}));
    EXPECT_EQ(halfSlice[1], (Cell{Material::air, 0.3, 300.0f, static_cast<Cell::Pressure>(halfSlice[1].GetPressure(0.0, earthGravity))}));
}

TEST(Stack, Find)
{
    EXPECT_FALSE(Stack(Material::stone).FindBiggestNonSolidOpening());
    EXPECT_EQ(Stack(Material::air).FindBiggestOpening().Size(), 1.0);
    EXPECT_RANGE_NEAR(Stack(
        {{Material::stone, 0.7, 300.0}, 
         {Material::air, 0.3, 300.0}, 
         {Material::stone, 0.5, 300.0}}).FindBiggestNonSolidOpening(), 
        Engine::Range<ZType>(0.7, 1.0), std::numeric_limits<ZType>::epsilon());
}

}
