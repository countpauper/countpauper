#include "Game/Block.h"
#include <gtest/gtest.h>
#include <cmath>

namespace Game::Test
{
using namespace ::testing;

TEST(Block, SolidLevel)
{
    EXPECT_TRUE(std::isnan(Block::Air.SolidLevel()));
    EXPECT_TRUE(std::isnan(Block::Water.SolidLevel()));
    EXPECT_EQ(Block::Stone.SolidLevel(), 1.0f);
    EXPECT_EQ(Block::Vegetation(0.1f, 0.5f).SolidLevel(), 0.5f);
    EXPECT_TRUE(std::isnan(Block::Vegetation(0.1f, 0.0f).SolidLevel()));
}

TEST(Block, LiquidLEvel)
{
    EXPECT_TRUE(std::isnan(Block::Air.LiquidLevel()));
    EXPECT_EQ(Block::Water.LiquidLevel(), 1.0f);
    EXPECT_FLOAT_EQ(Block(0.2f, 0.3f, 0.0f, 0.5f).LiquidLevel(), 0.8f);
    EXPECT_TRUE(std::isnan(Block::Stone.LiquidLevel()));
    EXPECT_TRUE(std::isnan(Block::Vegetation(0.1f, 0.5f).LiquidLevel()));

    // vegetation is fully submerged if less than water and raises the level
    EXPECT_FLOAT_EQ(Block(0.1f, 0.3f, 0.1f, 0.5f).LiquidLevel(), 0.9f);
    // vegetation is partially submerged if more than water
    EXPECT_EQ(Block(0.3f, 0.2f, 0.3f, 0.2f).LiquidLevel(), 0.6f);
}

TEST(Block, TopMaterial)
{
    EXPECT_EQ(Block::Air.GetMaterial(Orientation::up), Material::air);
    EXPECT_EQ(Block::Water.GetMaterial(Orientation::up), Material::water);
    EXPECT_EQ(Block::Stone.GetMaterial(Orientation::up), Material::stone);
    EXPECT_EQ(Block::Vegetation(0.1f, 0.5f).GetMaterial(Orientation::up), Material::vegetation);
    EXPECT_EQ(Block::Vegetation(0.0f, 0.5f).GetMaterial(Orientation::up), Material::stone);
}

TEST(Block, BottomMaterial)
{
    EXPECT_EQ(Block::Air.GetMaterial(Orientation::down), Material::air);
    EXPECT_EQ(Block::Water.GetMaterial(Orientation::down), Material::water);
    EXPECT_EQ(Block::Stone.GetMaterial(Orientation::down), Material::stone);
    EXPECT_EQ(Block::Vegetation(0.1f, 0.5f).GetMaterial(Orientation::down), Material::stone);
    EXPECT_EQ(Block::Vegetation(0.0f, 0.5f).GetMaterial(Orientation::down), Material::stone);
}

}
