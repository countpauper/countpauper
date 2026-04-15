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
    Map low(Engine::Size{1,1,1}, {{Material::stone, 8}});
    EXPECT_EQ(low.GroundHeight({0,0,0}), Position::ZType(0.5f));

    Map high(Engine::Size{2,2,16}, {
        {Material::stone, 200}, {Material::stone, 210},
        {Material::stone, 220}, {Material::stone, 240}});
    EXPECT_EQ(high.GroundHeight({0, 1, 14.1}), Position::ZType(13.75f));

}

TEST(Map, HeightMap)
{
    MockMap map;
    map.SetHeightMap(Engine::Size(2,2,3), 
        { 0.5, 1.8,
          2.1, 0.9 });
    EXPECT_EQ(map.GroundHeight({0, 0, 0}), Position::ZType(0.5));
    EXPECT_EQ(map.GroundHeight({1, 0, 2}), Position::ZType(1.8));
    EXPECT_EQ(map.GroundHeight({0, 1, 3}), Position::ZType(2.1));
    EXPECT_EQ(map.GroundHeight({1, 1, 1}), Position::ZType(0.9));
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
    // Use gMock's built-in FloatNear logic for the float members
    return ExplainMatchResult(FloatNear(expected.amount, epsilon), actual.amount, result_listener) && 
            ExplainMatchResult(FloatNear(expected.temperature, epsilon), actual.temperature, result_listener);
}



TEST(Map, GetSlice)
{
    NiceMock<MockMap> map;
    map.SetSize(Engine::Size(1,1,3));
    ON_CALL(map, GetBlock(Engine::Position(0,0,0))).WillByDefault(Return(Block(0.8)));
    ON_CALL(map, GetBlock(Engine::Position(0,0,1))).WillByDefault(Return(Block::Air));
    ON_CALL(map, GetBlock(Engine::Position(0,0,2))).WillByDefault(Return(Block::Stone));

    EXPECT_THAT(map.GetSlice({0,0,0}, 2.0), Pointwise(LayerEq(0.001f), 
        Slice( {{Material::stone, 0.8, 300.0f }, 
                {Material::air, 1.2, 300.0f }})));

    EXPECT_THAT(map.GetSlice({0,0,0.5}, 2.2),  Pointwise(LayerEq(0.001f),     
        Slice{{Material::stone, 0.3, 300.0f }, 
                {Material::air, 1.2, 300.0f },
                {Material::stone, 0.7, 300.0f }}));

}

}

