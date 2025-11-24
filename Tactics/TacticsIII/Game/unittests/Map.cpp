#include "Game/Map.h"
#include <gtest/gtest.h>


namespace Game::Test
{
using namespace ::testing;

TEST(Map, Anonymous)
{
    Map nemo(Engine::Size{});
    EXPECT_EQ(nemo.Name(), "map");
    EXPECT_EQ(nemo.FileName(), "");
}

TEST(Map, Bounds)
{
    Map zero(Engine::Size{});
    EXPECT_TRUE(zero.GetBounds().Empty());

    Map small(Engine::Size{1,1,1});
    EXPECT_EQ(small.GetBounds(), (Engine::IntBox{{0,1},{0,1},{0,1}}));

    Map big(Engine::Size{256,256,64});
    EXPECT_EQ(small.GetBounds(), (Engine::IntBox{{0,255},{0,255},{0,63}}));
}

TEST(Map, GroundHeight)
{
    Map low(Engine::Size{1,1,1}, {{Material::stone, 8}});
    EXPECT_EQ(low.GroundHeight({0,0,0}), 0.5f);

    Map high(Engine::Size{2,2,16}, {
        {Material::stone, 200}, {Material::stone, 210},
        {Material::stone, 220}, {Material::stone, 240}});
    EXPECT_FLOAT_EQ(high.GroundHeight({0, 1, 16}), 13.75f);

}

}

