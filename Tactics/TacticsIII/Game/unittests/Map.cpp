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

TEST(Map, Size)
{
    Map zero(Engine::Size{});
    EXPECT_TRUE(zero.GetBounds().Empty());

    Map small(Engine::Size{1,1,1});
    EXPECT_EQ(small.GetBounds(), (Engine::IntBox{{0,1},{0,1},{0,1}}));

    Map big(Engine::Size{256,256,64});
    EXPECT_EQ(small.GetBounds(), (Engine::IntBox{{0,255},{0,255},{0,63}}));
}

}

