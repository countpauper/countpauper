#include "Game/Map.h"
#include "Game/Mock/MockMap.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>




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
    EXPECT_EQ(small.GetBounds(), (Engine::IntBox{{0,0},{0,0},{0,0}}));

    Map big(Engine::Size{256,256,64});
    EXPECT_EQ(big.GetBounds(), (Engine::IntBox{{0,255},{0,255},{0,63}}));
}

TEST(Map, GroundHeight)
{
    Map low(Engine::Size{1,1,1}, {{Material::stone, 0.5}});
    EXPECT_EQ(low.GroundHeight({0,0,0}), ZType(0.5f));

    Map high(Engine::Size{2,2,16}, {
        {Material::stone, 12.5}, {Material::stone, 13.125},
        {Material::stone, 13.75}, {Material::stone, 15}});
    EXPECT_EQ(high.GroundHeight({0, 1, 14.1}), ZType(13.75f));

}

TEST(Map, HeightMap)
{
    MockMap map;
    map.SetHeightMap(Engine::Size(2,2,3), 
        { 0.5, 1.8,
          2.1, 0.9 });
    EXPECT_EQ(map.GroundHeight({0, 0, 0}), ZType(0.5));
    EXPECT_EQ(map.GroundHeight({1, 0, 2}), ZType(1.8));
    EXPECT_EQ(map.GroundHeight({0, 1, 3}), ZType(2.1));
    EXPECT_EQ(map.GroundHeight({1, 1, 1}), ZType(0.9));
}




MATCHER_P(LayerEq, epsilon, "") 
{
    // Pointwise passes a tuple: std::get<0> is actual, std::get<1> is expected
    const auto& [actual, expected] = arg; 
    
    if (actual.material.get() != expected.material.get())
    {
        *result_listener << "where the material didn't match (actual: " 
                         << actual.material.get().name << " vs expected: " << expected.material.get().name << ")";
        return false;
    }
    if (expected.amount != actual.amount)
        return false; // TODO explain or remove the whole thing 
    if (expected.temperature != actual.temperature)
        return false;   // TODO explain or just get rid of the LayerEq
    return true;
}



TEST(Map, GetSlice)
{
    NiceMock<MockMap> map;
    map.SetSize(Engine::Size(1,1,3));
    Slice slice({{Material::stone, 0.8, 300.0f},
                {Material::air, 1.2, 300.0f},
                {Material::stone, 1.0, 300.0f}});
    
    ON_CALL(map, SliceAt(0,0)).WillByDefault(ReturnRef(slice));

    EXPECT_THAT(map.GetSlice({0,0,0}, 2.0), Pointwise(LayerEq(0.001f), 
        Slice( {{Material::stone, 0.8, 300.0f }, 
                {Material::air, 1.2, 300.0f }})));

    EXPECT_THAT(map.GetSlice({0,0,0.5}, 2.2),  Pointwise(LayerEq(0.001f),     
        Slice{{Material::stone, 0.3, 300.0f }, 
                {Material::air, 1.2, 300.0f },
                {Material::stone, 0.7, 300.0f }}));

}

}

