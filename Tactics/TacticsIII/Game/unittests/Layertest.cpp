#include "Game/Layer.h"
#include <gtest/gtest.h>

namespace Game::Test
{
using namespace ::testing;

TEST(Layer, Vacuum)
{
    Layer vacuum;
    EXPECT_EQ(vacuum.material, Material::vacuum);
    EXPECT_EQ(vacuum.Volume(), 1.0f);
    EXPECT_EQ(vacuum.Mass(), 0.0f);
    EXPECT_TRUE(vacuum.IsTranslucent());
    EXPECT_FALSE(vacuum.IsGas());
    EXPECT_TRUE(vacuum.IsCompressible());
    EXPECT_FALSE(vacuum.IsSolid());
    EXPECT_FALSE(vacuum.IsLiquid());
}

TEST(Layer, Pressure)
{
    EXPECT_EQ(Layer(Material::vacuum).GetPressure(0.5), 0.0f);              // vacuum sucks
    EXPECT_NEAR(Layer(Material::air, 1.0, 305.15).GetPressure(0.0), atmosphericPressure, 0.5); // 1 atmosphere at 32 degrees
    EXPECT_TRUE(std::isnan(Layer(Material::stone).GetPressure(0.5))); 
    Layer air(Material::air, 1.0, 300.0);
    Layer warmAir = air;
    warmAir.Heat(20.0f);
    EXPECT_GT(warmAir.GetPressure(0.5), air.GetPressure(0.5));    // pressure higher for warmer air;
}

TEST(Layer, StaticPressure)
{
    Layer water(Material::water);
    EXPECT_GT(water.GetPressure(0.0), water.GetPressure(1.0));
    Layer deepWater(Material::water, 10.3, 277.0);
    EXPECT_GT(deepWater.GetPressure(0.0), water.GetPressure(0.0));
    EXPECT_NEAR(deepWater.GetPressure(0.0), 2 * atmosphericPressure, 1000);
}

TEST(Layer, Flow)
{
    // TODO: Could test that flow in negative direction is negated flow in the opposite direciton
    //  But not sure I like this interface yet. More important is that it's at the interface with the neighbour. 
    // Perhaps especially flow should be more structure of arrays instead of array of structure. Also so it can be handled like a 3d matrix 
}

}
