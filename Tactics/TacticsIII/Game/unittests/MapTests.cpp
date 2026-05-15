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

TEST(Map, EmptyMapIsAir)
{
    Map map(Engine::Size(1,1,3));
    EXPECT_EQ((map[0,0][0]).material.get(), Material::air);
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
    if (expected.height != actual.height)
    {
        *result_listener << "where the height didn't match (actual: " 
                         << actual.height << "m vs expected: " << expected.height << "m)";
        return false;
    }
    if (expected.temperature != actual.temperature)
    {
        *result_listener << "where the temperature didn't match (actual: " 
                         << actual.height << "K vs expected: " << expected.height << "K)";
        return false;   
    }
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

TEST(Map, Fill)
{
    Map map(Engine::Size{1,1,1});

    // Replace a (all) layer 
    auto it = map.Fill({0,0,0}, 1, Material::water);
    const auto& slice = map[0,0];
    ASSERT_EQ(slice.size(), 1);
    EXPECT_EQ(slice[0].material.get(), Material::water);
    EXPECT_EQ(slice[0].height, 1.0);
    EXPECT_EQ(slice[0].temperature, 300.0);
    EXPECT_EQ(it, slice.begin());

    // Shrink a layer and replace the top 
    it = map.Fill({0,0,0.6}, 0.4, Material::air);
    ASSERT_EQ(slice.size(), 2);
    EXPECT_EQ(slice[0].material.get(), Material::water);
    EXPECT_EQ(slice[0].height, 0.6);
    EXPECT_EQ(slice[1].material.get(), Material::air);
    EXPECT_EQ(slice[1].height, 0.4);
    EXPECT_EQ(it, map.SliceAt(0,0).begin() + 1);

    // Shrink a layer and replace the bottom and overlap a part of the top 
    map.Fill({0,0,0.5}, 0.125, Material::vegetation);
    ASSERT_EQ(slice.size(), 3);
    EXPECT_EQ(slice[0].height, 0.5);
    EXPECT_EQ(slice[1].material.get(), Material::vegetation);
    EXPECT_EQ(slice[1].height, 0.125);
    EXPECT_EQ(slice[2].height, 0.375);

    // Replace a whole layer 
    map.Fill({0,0,0.375}, 0.5, Material::water);
    ASSERT_EQ(slice.size(), 3);
    EXPECT_EQ(slice[0].height, 0.375);
    EXPECT_EQ(slice[1].material.get(), Material::water);
    EXPECT_EQ(slice[1].height, 0.5);

    // Split a layer 
    map.Fill({0,0,0.5}, 0.25, Material::earth);
    ASSERT_EQ(slice.size(), 5);
    EXPECT_EQ(slice[1].material.get(), Material::water);
    EXPECT_EQ(slice[1].height, 0.125);
    EXPECT_EQ(slice[2].material.get(), Material::earth);
    EXPECT_EQ(slice[2].height, 0.25);
    EXPECT_EQ(slice[3].material.get(), Material::water);
    EXPECT_EQ(slice[3].height, 0.125);
}

}

